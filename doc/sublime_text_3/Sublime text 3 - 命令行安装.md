系统ubuntu 16.04

1. 在命令终端依次输入以下命令即可安装sublime text3：
    
        wget -qO - https://download.sublimetext.com/sublimehq-pub.gpg | sudo apt-key add -
        sudo apt-get install apt-transport-https     
        echo "deb https://download.sublimetext.com/ apt/stable/" | sudo tee /etc/apt/sources.list.d/sublime-text.list    
        sudo apt-get update    
        sudo apt-get install sublime-text
