$script = <<SCRIPT
echo I am provisioning...
clang_vers="3.9"

apt-get update -y
apt-get install -y clang-${clang_vers} lldb-${clang_vers} make build-essential libc++-dev libssl-dev whois
ln -s /usr/lib/llvm-${clang_vers}/bin/clang++ /usr/local/bin/clang++
ln -s /usr/bin/lldb-${clang_vers} /usr/local/bin/lldb
SCRIPT

Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/xenial64"
  config.vm.define "dave-linux"
  config.vm.provision "shell", inline: $script
  config.vm.provider "virtualbox" do |v|
    v.memory = 1024
    v.cpus = 2
    v.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
  end
end
