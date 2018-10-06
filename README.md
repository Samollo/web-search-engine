# web-search-engine
Web Search Engine using C and Python. (Wrote in 2016)


# INSTALLATION & CONFIGURATION
First, make sure you have libcurl-dev installed on your computer, if not:
```
sudo apt-get install libcurl-dev
```
or
```
sudo apt-get install libcurl4-nss-dev
```

# RUN
First, you need to crawl some web page with this command:
```
make crawler
```
then you can run the search engine with:
```
make launcher
```
N.B: At the moment, user doesn't have a lot of control on which pages to crawl. It may change in the futur.
