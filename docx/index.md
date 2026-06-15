---
# https://vitepress.dev/reference/default-theme-home-page
layout: home

hero:
  name: "拟声 • Musicxx"
  text: "本地/云盘/NAS全格式音视频播放器"
  tagline: 多系统/音源支持、歌词弹幕、跨设备共享与控制
  actions:
    - theme: brand
      text: 下载
      link: https://download.music.bool.run/
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
    details: 拟声创新功能, 将歌词作为弹幕飘过! 降低歌词对其他App内容的遮挡. 支持桌面歌词和状态栏歌词（悬浮窗/系统级）
  - title: 多音源支持
    details: 支持 本地/Bilibili/WebDav/阿里云盘/百度云盘/115云盘/Subsonic/Jellyfin/Emby 音视频; 几乎支持全音视频格式播放
  - title: 多系统支持
    details: 安卓/IPhone/Windows/Macos/Linux; 支持车机电视, 自动横竖屏兼容
  - title: 共享与控制
    details: 同一局域网内, 可自动将其他设备的本地歌曲和缓存共享过来播放! 可以远程控制其他设备, 且支持 AI/MCP 接入控制
  - title: Mediaxx
    details: C++实现的开源高性能模块, 接管 封面信息提取/音乐动效频谱分析/字符编码检测和转换 等高负载功能; 合并 ffmpeg/libmpv 编译+多种编译优化, 扩展全格式支持的同时大幅缩减体积
    link: https://github.com/coolight7/mediaxx
  - title: 智能识别
    details: 自动从Bili视频标题提取歌曲名和歌手; 支持关联 歌手名 的网名、简写名、繁简体、错拼名; 几乎兼容所有 多歌手分割符, 自动修正包含分隔符的歌手名
  - title: 音乐动效
    details: 音频可视化, 且支持类似录音的频谱进度条, 以便预览歌曲副歌位置
  - title: 音效
    details: 兼容耳机/扬声器等任意设备支持, 均衡器EQ、声场扩展、回声、低音增强、3D环绕、动态扩展压缩、人声增强、人声消除、强制响度平衡、跳过静音段
  - title: 音源转换
    details: 自动关联网络歌曲、本地歌曲和缓存, 局域网自动共享, 减少流量消耗
  - title: 播放记录扩展
    details: 扩展播放次数、播放进度、搜索、分类、追溯所在歌单; 关联所有音源歌曲, 归类 歌手/专辑 分类; 让 webdav 等音源支持搜索
  - title: 同步音源/分组创建歌单
    details: 歌单配置音源后, 可以点击同步一键添加新增歌曲; 支持根据音源文件夹结构自动分类创建歌单并绑定音源, 以便后续同步
  - title: 网络请求合并
    details: 支持自动合并播放/提取封面信息请求, 大幅降低流量消耗, 支持预加载音频, 提高播放速度
  - title: 首字母索引优化
    details: 即便是按时间等方式排序的 非名称排序列表, 也支持名称滑动索引定位
  - title: 丝滑动画效果
    details: 多种 Q弹的回弹动画、粒子动画, 全APP动画按性能负担分级, 可根据设备性能调整启用动画级别
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
