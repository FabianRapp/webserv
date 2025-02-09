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

curl http://magicform.com:8080 --resolve magicform.com:8080:127.0.0.1

curl http://www.getbetter.com:8080 --resolve www.getbetter.com:8080:127.0.0.1

curl http://magicform.com:8080 --resolve magicform.com:8080:127.0.0.1

curl http://magicform.com:9090 --resolve magicform.com:9090:127.0.0.1

curl http://www.^-getbetter.com:9090 --resolve www.^-getbetter.com:9090:127.0.0.1

curl http://www.getbetter.com:9090 --resolve www.getbetter.com:9090:127.0.0.1

telnet?


----------------
CGI:
You need to test with files containing errors to see if the error handling works properly. You can use a script containing an infinite loop or an error; you are free to do whatever tests you want within the limits of acceptability that remain at your discretion. The group being evaluated should help you with this.



------------------

server_name validation.
request_body_size should be in location also.
add PUT method to allowed_methods for both.


bool CGIManager::isCGI(const std::string& path, const LocationConfigFile& location_config) {
	finish validating .py | .php | .bla
