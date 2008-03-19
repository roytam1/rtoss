<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:atom="http://purl.org/atom/ns#" 
	xmlns="http://www.w3.org/1999/xhtml">
	
	<xsl:template match="/">
		<xsl:apply-templates select="rss" />
	</xsl:template>
	
	<xsl:template match="rss">
		<html>
		<head>
    		<title>RSS 2.0 Feed of <xsl:value-of select="channel/title"/></title>
  		</head>
  		<body>
  		
		<xsl:variable name="tdesc" select="channel/description" />
		<xsl:variable name="thref" select="channel/link" />
		<xsl:variable name="ttitle" select="channel/title" />
		<p>This is the RSS 2.0 feed of <a href="{$thref}" title="{$ttitle}: {$tdesc}"><xsl:value-of select="channel/title" /></a> in total of <xsl:value-of select="count(channel/item)"/> items and order by Category and ID.</p>
		
		<xsl:for-each select="channel/item">
			<xsl:variable name="href"><xsl:value-of select="link" /></xsl:variable>
			<xsl:variable name="title"><xsl:value-of select="title" /></xsl:variable>
			<xsl:variable  name="idesc" select="description" />
			<h4><a href="{$href}" title="{$title}: {$idesc}">
				<xsl:value-of select="$title" />
			</a></h4>
		</xsl:for-each>
		</body>
  		</html>
	</xsl:template>
</xsl:stylesheet>