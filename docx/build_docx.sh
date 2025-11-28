npm run docs:build

rm -rf build/

mkdir build

mv .vitepress/dist/* build/

mkdir build/images/

cp -r images/ build/
