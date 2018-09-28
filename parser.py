#!/usr/bin/env python2.7
from bs4 import BeautifulSoup
from urllib2 import * 
import urllib2
import unicodedata
import string
import re 
import requests 
import gzip 


def getTitle(url, rien):
	html = requests.get(url)
	soup = BeautifulSoup(html, "html.parser")
	return soup.title.string

def isStopWord(mot):
	with open('stopwords.txt',"r") as f:
		for ligne in f.readlines():
			if(mot in ligne):
				return True
	return False

def setFrequenceMot(tMot):
	dFrequence = {}
	for m in tMot:
		if dFrequence.has_key(m):
			dFrequence[m] = dFrequence[m] + 1
		else:
			dFrequence[m] = 1
	listeFrequenceMot = convertDicoToList(dFrequence)
	return listeFrequenceMot

def convertDicoToList(dico): 
	liste = []
	for key, value in dico.items():
		liste.append(key)
		liste.append(str(value))
	return liste

def delAccentUppercase(liste): 
	end=[]
	for mot in liste:
		g= (x for x in unicodedata.normalize('NFKD',mot) if unicodedata.category(x) != 'Mn')
		end.append(''.join(c for c in g if c in string.ascii_letters).lower())
	return end

def extractWord(path, rien): 
	soup = BeautifulSoup(open(path), "html.parser")
	for script in soup(["script", "style"]):
		script.extract()    
	for i in soup.findAll('time'):
		i.extract() 

	text = soup.get_text() 

	lines = (line.strip() for line in text.splitlines())
	chunks = (phrase.strip() for line in lines for phrase in line.split("  ")) 
	text = '\n'.join(chunk for chunk in chunks if chunk)
	tmp = unicodedata.normalize('NFKD', text).encode('ascii', 'ignore').lower().strip(string.punctuation)
	temporaryList = tmp.split()
	wordList = []
	for word in temporaryList: 
		if isStopWord(word) == False: 
			m = word.split("'")
			for s in m:
				s = s.strip(string.punctuation)
				s = re.sub('[!"#$%&()*+,-./:;<=>?@[\]^_{|}~]', '', s) 
				if len(s) > 2 and len(s) < 51: 
					wordList.append(s)	
	return setFrequenceMot(wordList) 

def geturls(path, url): 
	soup = BeautifulSoup(open(path), "html.parser")
	urlToVisit = []
	i = 0
	for link in soup.find_all('a', href=True):
		tmp = link['href']
		if tmp.startswith('http') or tmp.startswith('fttp') or tmp.startswith('https') or tmp.startswith('file://') or tmp.startswith('/'):
			if tmp.startswith('/'):
				if url.endswith('/'):
					url.pop(len(url)-1)
			tmp = url + tmp
			if not tmp.endswith('php') and not tmp.endswith('pdf') and not tmp.endswith('xls') and (tmp.endswith('.fr') or tmp.endswith('.com') or tmp.endswith('.uk') or tmp.endswith('.html')):
					if tmp.endswith('htm'): #Si l'url termine par pdf par exemple, on ne pourras pas la lire car ce n'est pas html, mais si elle termine par .fr on pourra
						tmp = tmp + 'l' #Si l'url termine par htm, on lui rajoute le l manquant
					urlToVisit.append(link.get('href'))
					i = i+1	
	return urlToVisit

def textbrut(path):
	soup = BeautifulSoup(open(path))
	contenuPage = soup.get_text()
	print(contenuPage)
	return contenuPage


if __name__ == '__main__':
	print(string.punctuation)
    	

