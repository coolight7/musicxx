---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: "拟声"
  text: "拟物风音视频播放器"
  tagline: 安卓/Windows/macos/linux、歌词弹幕、跨设备共享与控制、WebDAV/阿里云盘/百度云盘
  actions:
    - theme: brand
      text: 下载
      link: https://download.music.mimicry.cool/
    - theme: alt
      text: 使用帮助
      link: /help/
    - theme: alt
      text: GitHub
      link: https://github.com/coolight7/MimicryMusic
  image:
      src: images/logo-tran.png
      alt: 拟声

features:
  - title: 多系统平台支持
    details: 目前已支持安卓/windows/macos/linux，未来可提供IPhone/鸿蒙支持
  - title: 歌词弹幕
    details: 拟声特色功能，将歌词作为弹幕发送到屏幕上！降低歌词对其他App内容的遮挡。也支持桌面歌词和状态栏歌词（悬浮窗/系统级）
  - title: WebDav/阿里云盘/百度云盘
    details: 支持多种云存储，当然也支持本地音视频播放。
  - title: 跨设备共享与控制
    details: 同一局域网内，可将其他设备的本地歌曲和缓存共享过来播放！可以远程控制其他设备。
---
<style>
:root {
  --vp-home-hero-name-color: transparent;
  --vp-home-hero-name-background: -webkit-linear-gradient(120deg, #66ccff 30%, #41d1ff);

  --vp-home-hero-image-background-image: linear-gradient(-45deg, #e1edfa 50%, #fff9ec 50%);
  --vp-home-hero-image-filter: blur(44px);
}

html.dark {
  --vp-home-hero-image-background-image: linear-gradient(-45deg, #13e4ea 50%, #7d78eb 50%);
}

@media (min-width: 640px) {
  :root {
    --vp-home-hero-image-filter: blur(56px);
  }
}

@media (min-width: 960px) {
  :root {
    --vp-home-hero-image-filter: blur(68px);
  }
}
</style>
