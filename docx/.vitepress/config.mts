import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  lang: 'zh-Hans',
  title: "拟声",
  description: "拟物风音视频播放器",
  appearance: "dark",
  lastUpdated: true,
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
          { text: '拟声++', link: '/musicPlusPlus/' },
          { text: '开发记录', link: '/develop/' },
        ]
      }
    ],
    footer: {
      message: `
<a target="_blank" href="https://beian.miit.gov.cn/">粤ICP备2021172577号</a>
<a style="display: flex;flex-direction: row;justify-content: center;" target="_blank" href="http://www.beian.gov.cn/portal/registerSystemInfo?recordcode=44011302003939">
    <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAUCAYAAACNiR0NAAAFOklEQVR42k3UaUzTZwDH8UfmNqPG6Oamc04lM/OYZs4DxQw85vBGBQ/KiiAqiop4C4yhAtPMA8VbZ2SbJo7JVFSYKCAooCCoUCkF5JCWo7S0lZa2XH7X8QJ48fvn/yRPPs/veZ7kEYCgydSR9iazwGQS5haFAIMA1eCmsjt+d0+HPfvnSEh+Q37sFigaCs3CarUK9HrBW4MtjR0BRMenpckgWsx60WbUCZqxJX1Wya2fYmM2rrBc9p7AAY/pRPov4MrPblz0l7bK48Ju0Pxy0TuzQbTrVaLNUG2LqgvUmpVC11QhWtsRYOmVcmI10z6dyikfBzI2T6La50u028dTluhP3J87mf6RI0lHPADjh8amamEyxAuLKb4b2FwtDNYGAdqh1uch6a5TlhAdIsEYOhpZz14UOI6g2HUsz+160BrjQuzdaJZPc+ed4vhtKOtltqYIuNcFWhsVopXyMW/ubGx2GLKY1RI/2qLGUWA/hEL7wbwYN4TiuaN5MWgA6X37QKoPm3dGMnvkHOofh9Roi4+Nkidv6gLRvBBYzk05fXQ3Qhzg2jEJhtXDePVeH+TfDkc+ZBD5fQfwatpIsoUdGu8xpNwPQ4itHDu4F2tJ+AR56sUu0CB/MLT94fLHJ4+FIXruJWH/DKpchlEsPkA+fBBFTvYUzbC3/X/GCyEoch6BIncvokcgoSHBoD+Y+q5Fa98J5t/YE2i56siVC5EIEcbdCx6Ywx3IE++TLRnLq7NOKNOWURk1l/xPetOwfTJ5j7Z3NDxzIgJkK9GW3Y7oBEvvHzqvPjeZzBuhiF7RbPX1pSVjDvcXjyTL+QsKN06kePd0SgOmUnTYBarWErzOByGCyUw5hOnWTFTpx2M6weK/AqpLD07jbU4gQ53P0fP9AB4sGk19uBOF88by7P8te09CNvMrVIEOyNY70eeDDQya8CuNiiAqzzhScXNDaVfDON+m50HTIWE+O3453bFyouhP+9e9KVr7DZluIyncPZXa8Bm0jx/IQzEAITbhHxYFme7IDjmhTPav7ARVifvjy2Jm05a7DlnyIcTnUSwSjsT360fG4N5kjfqYXFtKFo4h13k8/nYOiP4h5CQG0VoQiPL2chryDp7pBoY/eR09i6rYVVDgR+TxCx23LXr4IcQaXPu54jNsKX2FF0JssCWIHSHhUOJFdYIXyr8XoMkOzugE67JCWxWXPSiKdkd5yonGZwHI04I4vVfKluDD3L3iR16cG1LvIFa6BrJzTzgNj3yp+92J0t9+QHlzIfrifdpO0GrM9qxOduP11UUUnphL4a7RWDM8oNQHFItpy1yG4eY8eOkJ+gCQSSiPmGib68Kb227U50hp1iU4d4KAaDW9lKqfrjbUpK2gIMIJedR36PLWo3kgpSZ+GXUPJBjy1vFW5k/5BRdKzi5AnROA6lGA1qi+Puddc1K358tcKd5WPJn15taGWvUTKerMNdQkrkSTtgpNphfq9DXoc9ZQn+JBxR9zqbknQfdqG7UZfry+JtE0VMTMMVu6gc2GVGGqOHLfUrULnWI3VYneqO54ok7xRHlrKeo0b9T/ulF+/nsqbcdSk+RJja25Nm8jRtUOjOqT1026biDGXNHamNa7XhFxSpUsMdUlLaExeyW6VBsWvxCtbVx3Y35HY91TT+oeulKf40tD+f43+roroRbjPTuL6Wo3sPGJaLNkiNeZUUJ2atvAnMgfvR7vc75UFuueVXppWYEyzl1VmyBVlV+TZtUmb0nWFJw/WnwzfN7L69vtNPVJwmpOFhbjpQ7rPyXTHALIQZVcAAAAAElFTkSuQmCC"
        style="width:20px;height:20px;margin-right:10px;">
    <span >粤公网安备 44011302003939号</span>
</a>
`,
      copyright: `版权所有 © 2022-${new Date().getFullYear()} <a href="https://github.com/coolight7">『coolight • 郑泳坤』</a> `
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/coolight7/MimicryMusic' }
    ],
    notFound: {
      title: "页面未找到",
      quote: "这可能是因为文章迁移，您可以通过本页面左上角的搜索框查找相关内容。",
      linkLabel: "回到主页",
      linkText: "回到主页",
    },
    search: {
      provider: 'local',
      options: {
        translations: {
          button: {
            buttonText: '搜索文档',
            buttonAriaLabel: '搜索文档'
          },
          modal: {
            resetButtonTitle: '清除查询条件',
            backButtonTitle: '取消',
            noResultsText: "无搜索结果",
            footer: {
              selectText: '选择',
              navigateText: '切换',
              closeText: '关闭',
            },
          }
        }
      }
    },
    outlineTitle: "本页",
    lastUpdatedText: "最后更新",
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
    darkModeSwitchTitle: '切换到深色模式',
  }
})
