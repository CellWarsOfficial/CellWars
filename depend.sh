#!/bin/bash


apt-get --assume-yes update
echo "installing NPM and nodeJS, required for reactJS."
apt-get --assume-yes install nodejs 
apt-get --assume-yes install npm
ln -s /usr/bin/nodejs /urs/bin/node
echo "updating nodeJS to latest version using n."
cd react/my-app/
npm install $1 n
npm install $1 npm@latest
n stable
echo "updating npm to latest version."
npm update
npm cache clean --force
echo "downloading reactJS."
npm install $1 react
npm install $1 react-dom
npm install $1 create-react-app
npm install $1 react-scripts
echo "updating again to make sure"
npm update
