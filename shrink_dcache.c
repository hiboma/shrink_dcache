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

static struct dentry *debugfs_file;

static int try_shirink_dcache(void)
{
        int err;
        struct path path;

	err = kern_path(buffer, 0, &path);
	if (err)
		return err;

	dentry_unhash(path.dentry);
	path_put(&path);

        return 0;
}

static ssize_t shrink_dcache_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
        int error;
        ssize_t ret;
        char buffer[128];

        ret = simple_write_to_buffer(buffer, sizeof(buffer), pos, buf, count);
        if (ret < 0) { 
           pr_info(">> 1");
           return ret;
        }

        buffer[ret] = '\0';
        if (buffer[ret-1] == '\n') 
          buffer[ret-1] = '\0';

        pr_info("path is %s", buffer);
        error = try_shirink_dcache(buffer);
        if (error) {
           pr_info(">> 3");
            return -error;
         }

        return ret;
}

static const struct file_operations fops = {
	.write = shrink_dcache_write,
};

static int __init shrink_dcache_init(void)
{
	debugfs_file = debugfs_create_file("shrink_dcache", 0400,
					   NULL, NULL, &fops);
	if (debugfs_file == NULL) {
		pr_info("failed to debugfs_create_dir('shrink_dcache')");
		return -ENOMEM;
	}

        return 0;
}

static void __exit shrink_dcache_exit(void)
{
        debugfs_remove(debugfs_file);
}

module_init(shrink_dcache_init);
module_exit(shrink_dcache_exit);
