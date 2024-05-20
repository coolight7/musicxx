import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "拟声",
  description: "拟物风音视频播放器",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    nav: [
      { text: '主页', link: '/' },
      { text: '下载', link: 'https://download.music.mimicry.cool/' },
      { text: '了解拟声', link: '/info/' },
      { text: '使用帮助', link: '/help/' },
      { text: '关于作者', link: '/about/author' },
    ],

    sidebar: [
      {
        text: '目录',
        items: [
          { text: '下载', link: 'https://download.music.mimicry.cool/' },
          { text: '了解拟声', link: '/info/' },
          { text: '使用帮助', link: '/help/' },
          { text: '拟声++', link: '/mymusicPlusPlus/' },
          { text: '开发记录', link: '/develop/' },
        ]
      }
    ],

    footer: {
      copyright: `版权所有 © 2022-${new Date().getFullYear()}『coolight • 郑泳坤』`
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/coolight7/MimicryMusic' }
    ],

    docFooter: {
      prev: '上一页',
      next: '下一页'
    },

    outline: {
      label: '本页导航'
    },
    
    langMenuLabel: '多语言',
    returnToTopLabel: '回到顶部',
    sidebarMenuLabel: '菜单',
    darkModeSwitchLabel: '主题',
    lightModeSwitchTitle: '切换到浅色模式',
    darkModeSwitchTitle: '切换到深色模式'
  }
})