#include <linux/module.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/dcache.h>
#include <linux/debugfs.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("hiboma <hiroyan@gmail.com>");
MODULE_DESCRIPTION("This module is PoC");

static struct dentry *dentry_dcache, *dentry_sb_cache;

static int try_shrink_super_block_cache(const char *buffer)
{
	int err;
	struct path path;
	struct super_block *sb;

	err = kern_path(buffer, 0, &path);
	if (err)
		return err;

	sb = path.dentry->d_sb;
	if (!sb_rdonly(sb)) {
		pr_err("%s on %s is not SB_RDONLY", sb->s_id, buffer);
		path_put(&path);
		return -EINVAL;
	}

        pr_info("%s", sb->s_id);

	down_write(&sb->s_umount);

	shrink_dcache_sb(sb);
	evict_inodes(sb);

	up_write(&sb->s_umount);
	path_put(&path);

        return 0;
}

static int try_shirink_dcache(const char *buffer)
{
	int err;
	struct path path;

	err = kern_path(buffer, 0, &path);
	if (err)
		return err;

	shrink_dcache_parent(path.dentry);
	path_put(&path);

	return 0;
}

static ssize_t shrink_super_block_cache(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
        int error;
        ssize_t ret;
        char buffer[128];

	ret = simple_write_to_buffer(buffer, sizeof(buffer), pos, buf, count);
	if (ret < 0)
		return ret;

	buffer[ret] = '\0';
	if (buffer[ret-1] == '\n')
		buffer[ret-1] = '\0';

	error = try_shrink_super_block_cache(buffer);
	if (error)
		return error;

	return ret;
}

static ssize_t shrink_dcache_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
        int error;
        ssize_t ret;
        char buffer[128];

	ret = simple_write_to_buffer(buffer, sizeof(buffer), pos, buf, count);
	if (ret < 0)
		return ret;

	buffer[ret] = '\0';
	if (buffer[ret-1] == '\n')
		buffer[ret-1] = '\0';

	error = try_shirink_dcache(buffer);
	if (error)
		return error;

	return ret;
}

static const struct file_operations dcache_fops = {
	.write = shrink_dcache_write,
};

static const struct file_operations sb_cache_fops = {
	.write = shrink_super_block_cache,
};

static int __init shrink_dcache_init(void)
{
	dentry_dcache = debugfs_create_file("shrink_dcache", 0400,
					   NULL, NULL, &dcache_fops);
	if (dentry_dcache == NULL) {
		pr_err("failed to debugfs_create_dir('shrink_dcache')");
		return -ENOMEM;
	}

	dentry_sb_cache = debugfs_create_file("shrink_super_block_cache", 0400,
					   NULL, NULL, &sb_cache_fops);
	if (dentry_sb_cache == NULL) {
		debugfs_remove(dentry_dcache);
		pr_err("failed to debugfs_create_dir('shrink_super_block_cache')");
		return -ENOMEM;
	}

	return 0;
}

static void __exit shrink_dcache_exit(void)
{
	debugfs_remove(dentry_dcache);
	debugfs_remove(dentry_sb_cache);
}

module_init(shrink_dcache_init);
module_exit(shrink_dcache_exit);
