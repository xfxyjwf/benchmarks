sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu/ trusty main restricted" -y
sudo apt-get update -qq

# Install g++-4.8
sudo apt-get install -qq g++-4.8

# Install cmake.
sudo apt-get install -qq cmake

# Install thrift deps.
sudo apt-get install -qq libboost-dev libboost-test-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev
