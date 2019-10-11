# -*- mode: ruby -*-

$script = <<SCRIPT
: install
(
  sudo apt update
  sudo apt upgrade
  sudo apt install linux-image-4.15.0-65-generic
  sudo apt install -y linux-headers-$(uname -r)
  sudo apt install -y gawk wget git diffstat unzip texinfo gcc-multilib \
           build-essential chrpath socat libsdl1.2-dev xterm libncurses5-dev \
           lzop flex libelf-dev kmod

  git clone https://github.com/hiboma/shrink_dcache
  cd shrink_dcache
  make
)
SCRIPT


Vagrant.configure("2") do |config|

  config.vm.define "xenial" do |c|
    c.vm.box      = "bento/ubuntu-16.04"
    c.vm.hostname = "xenial"
  end

  config.vm.provision "shell", inline: $script

  # config.disksize.size  = '100GB'
  config.ssh.insert_key = false
  config.vm.provider "virtualbox" do |vb|
    vb.gui = true
    vb.customize ["modifyvm", :id, "--memory", "5000", "--cpus", "2", "--ioapic", "on"]
    # vb.customize ["modifyvm", :id, "--uart1", "0x3F8", "4"]
    # vb.customize ["modifyvm", :id, "--uartmode1", "file", File.join(Dir.pwd, "console.log") ]
    # vb.customize ["modifyvm", :id, "--uartmode1", "file", "/tmp/vagrant-ubuntu-ttyS0.log"]
    # vb.customize ["modifyvm", :id, "--uart2", "0x2F8", "3"]
    # vb.customize ["modifyvm", :id, "--uartmode2", "server", "/tmp/vagrant-ubuntu-ttyS1"]
  end
end
