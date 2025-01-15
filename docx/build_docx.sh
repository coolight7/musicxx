npm run docs:build

rm -rf build/

mkdir build

mv .vitepress/dist/* build/

mkdir build/images/

cp images/logo-tran.png build/images/logo-tran.png