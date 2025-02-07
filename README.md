# webserv


1. CGI variables.
2. HTML file to select file to upload.
3. CGI script to return 505 file or some kinda of simple .html
4. GCI GET and POST. Scripts to to that.

And config file for those CGI locations.


------------------

How to use CURL:
https://acquia.my.site.com/s/article/360005257154-Use-cURL-s-resolve-option-to-pin-a-request-to-an-IP-address

./webserv config/cgitest.conf
curl http://www.kotlet.com:9090 --resolve www.kotlet.com:9090:127.0.0.1
curl http://hello.com:8080 --resolve hello.com:8080:127.0.0.1


telnet?
