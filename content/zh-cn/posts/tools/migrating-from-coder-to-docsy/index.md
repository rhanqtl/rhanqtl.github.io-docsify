---
title: "博客主题迁移：从 Coder 到 Docsy"
date: 2023-04-15T18:14:43+08:00
tags: ["Hugo"]

draft: false
---

# 缘起

最近写博客的内容格式从 Markdown 切到了 AsciiDoc[^asciidoc]，发现目前用的 Coder 主题不支持这种格式[^hugo-coder-asciidoc-support]。网上有一些讲如何通过自定义 CSS 让 Coder 主题支持 AsciiDoc 的博客[^hugo-coder-custom-css-asciidoc]，然而尝试过后发现效果很一般，我也不是非常有兴趣花很多时间学 CSS，就产生了切主题的想法。经过简单搜索发现 Docsy[^hugo-theme-docsy] 能够支持 AsciiDoc。

[^asciidoc]: asciidoc.org

[^hugo-coder-asciidoc-support]: 参见 [issue #784](https://github.com/luizdepra/hugo-coder/issues/784)

[^hugo-coder-custom-css-asciidoc]: https://rgielen.net/posts/2019/creating-a-blog-with-hugo-and-asciidoctor/

[^hugo-theme-docsy]: https://www.docsy.dev/

本文用以记录迁移过程

# 迁移过程

总的来说改动不大，但是花的时间确实比我预计的要多，归根结底还是对 Hugo 本身的各种基础概念没有很好的了解。

使用 Docsy 需要安装一些依赖，这部分可以参考[这里](https://blog.tangly.net/blog/2020/creating-a-technical-website-with-hugo-and-asciidoc/#_install_the_tools)

接下来需要做的是修改配置，这部分抄一抄官方的[示例](https://github.com/google/docsy/blob/1d126ab1bbf186fa0ea52a6d6a3f58229b2b7942/config.toml)就可以了。

再然后需要加一些 `_index.md` 文件配置导航栏。这里需要先了解 Hugo 中“[section](https://gohugo.io/content-management/sections/)”的概念。按照[示例站点](https://github.com/google/docsy-example/tree/master/content)的结构抄一下，你可能会发现导航栏没有显示出来想要的链接，这可能是因为没有对应的 template，比如我想要用 `posts` 这个 section 代替默认的 `blog` 来管理所有的博文，那么我就需要把 `docsy/layouts/blog` 的部分拷贝到博客根目录的 `layouts` 目录下。

# 心得

总而言之还是要仔细读文档。

不过不得不吐槽的是 Hugo 搞出了太多的概念，有的时候为了扩展还必须要去修改主题的源文件，对于我这种想要开箱即用的用户来说不是很友好。


