<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:rss="http://purl.org/rss/1.0/"
  xmlns:dc="http://purl.org/dc/elements/1.1/"
  xmlns:content="http://purl.org/rss/1.0/modules/content/"
  exclude-result-prefixes="rdf rss dc content"
>

<xsl:template match="/">
  <xsl:apply-templates select="rdf:RDF"/>
</xsl:template>

<xsl:template match="rdf:RDF">
  <html>
  <head>
    <title><xsl:value-of select="rss:channel/rss:title"/></title>
    <meta http-equiv="Content-Script-Type" content="text/javascript" />
    <link rel="shortcut icon" href="favicon.ico" />
  </head>
  <body>
    <h1><a href="{rss:channel/rss:link}"><xsl:value-of select="rss:channel/rss:title"/></a></h1>

    <p>This is the RSS 1.0 feed of <a href="{rss:channel/rss:link}"><xsl:value-of select="rss:channel/rss:title"/></a> in total of <xsl:value-of select="count(rss:item)"/> items and order by Category and ID.</p>

    <xsl:apply-templates select="rss:item"/>

    <!-- Credits: 斎藤 宏明 (sonic64@infoseek.jp) -->
  </body>
  </html>
</xsl:template>

<xsl:template match="rss:item">
    <div><h4><a href="{rss:link}"><xsl:value-of select="rss:title"/></a></h4>
    </div>
</xsl:template>

</xsl:stylesheet>