<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:atom="http://purl.org/atom/ns#" 
	xmlns="http://www.w3.org/1999/xhtml">
	
	<xsl:template match="/">
		<xsl:apply-templates select="atom:feed" />
	</xsl:template>
	
	<xsl:template match="atom:feed">
		<html>
		<head>
    		<title>Atom 0.3 Feed of <xsl:value-of select="atom:title"/></title>
  		</head>
  		<body>
  		
		<xsl:variable name="thref" select="atom:link/@href" />
		<xsl:variable name="ttitle" select="atom:title" />
		<xsl:variable name="tdesc"  select="atom:tagline" />
		<p>This is the Atom 0.3 feed of <a href="{$thref}" title="{$ttitle}: {$tdesc}"><xsl:value-of select="atom:title" /></a> in total of <xsl:value-of select="count(atom:entry)"/> items and order by Category and ID.</p>
		
		<xsl:for-each select="atom:entry">
			<xsl:variable name="href" select="atom:link/@href" />
			<xsl:variable name="title" select="atom:title" />
			<xsl:variable name="idesc" select="atom:content" />
			<h4><a href="{$href}" title="{$title}: {$idesc}">
				<xsl:value-of select="$title" />
			</a></h4>
		</xsl:for-each>
		</body>
  		</html>
	</xsl:template>
</xsl:stylesheet>