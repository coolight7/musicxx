---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: "拟声"
  text: "拟物风本地/云盘/NAS音视频播放器"
  tagline: 安卓/IPhone/Windows/Macos/Linux、歌词弹幕、跨设备共享与控制、WebDAV/阿里云盘/百度云盘/115云盘/Subsonic/Jellyfin/Emby
  actions:
    - theme: brand
      text: 下载
      link: https://download.music.mimicry.cool/
    - theme: alt
      text: 使用帮助
      link: /help/
    - theme: alt
      text: GitHub
      link: https://github.com/coolight7/musicxx
  image:
      src: images/musicxx-glass-blue.png
      alt: 拟声

features:
  - title: 歌词弹幕
    details: 拟声创新功能，将歌词作为弹幕飘过！降低歌词对其他App内容的遮挡。也支持桌面歌词和状态栏歌词（悬浮窗/系统级）
  - title: WebDav/阿里云盘/百度云盘/115云盘/Subsonic/Jellyfin/Emby
    details: 支持多种云盘、Nas 音乐库，也支持本地音视频；几乎支持全音视频格式播放
  - title: 多系统支持
    details: 目前已支持 安卓/IPhone/Windows/Macos/Linux
  - title: 共享与控制
    details: 同一局域网内，可自动将其他设备的本地歌曲和缓存共享过来播放！可以远程控制其他设备，且支持 AI/MCP 接入控制
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
