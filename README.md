<h1>🌐 42-Webserv 🌐</h1>
Webserv is a web server project developed as part of the <a href="https://www.42network.org/">42 School</a> curriculum, compliant with HTTP 1.1 and REST and based on Nginx both in features and configuration files

<h2>✅ Features ✅</h2>
<ul>
  <li><b>HTTP/1.1 Compliance</b> - Compliant with HTTP 1.1 protocol including request parsing, response formatting, and persistent connections</li>
  <li><b>Multi-client Support</b> - Using the epoll kernel API it ensures multiplexing to serve multiple clients performantly</li>
  <li><b>Static & Dynamic Content</b> - Serves both static files and executes CGI scripts in multiple languages for dynamic content generation</li>
  <li><b>Error Handling</b> - Returns descriptive HTTP/WebDAV codes and allow the user to customize it's error pages</li>
  <li><b>Standard Configuration</b> - Configuration file directives based on Nginx for ease of use</li>
  <li><b>Basic features</b>:
    <ul>
    <li>GET, POST, DELETE methods support</li>
    <li>Autoindexing for directories</li>
    <li>Fully implemented location trees in the configuration file</li>
    <li>File upload handling and collection endpoint support</li>
    <li>Configurable request body size limit and allowed methods per location for better security</li>
    <li>Customizable error pages</li>
    <li>Accurate MIME type identification and return</li>
    </ul>
  </li>
  <li><b>Memory safe</b> - Does not contain leaks and does not crash</li>
</ul>

<h2>🛠️ Compilation and usage 🛠️</h2>

1. **Clone the repository:**
   ```bash
   git clone https://github.com/princess-mikus/42-Webserv.git
   cd 42-Webserv
   ```

2. **Compile the project:**
   ```bash
   make
   ```

3. **Configure the server:**
   Edit the provided sample configuration file in the config folder to set up server blocks, ports, root directories, error pages, and CGI paths.

4. **Run the server:**
   ```bash
   ./webserv path/to/config.conf
   ```

5. **Test the server:**
   - Open your browser and navigate to the configured host/port.
   - Use tools like `curl` or Postman to test HTTP methods.

<h2>Project Structure</h2>

- `src/` — Source code files
- `conf/` — Example configuration files
- `www/` — Static website content and CGI scripts
- `Makefile` — Build instructions


<h2>Made by:</h2>
 <table>
  <tr>
    <th><a href="https://www.github.com/princess-mikus"><img src="https://avatars.githubusercontent.com/u/46509432?v=4"></a></th>
    <th><a href="https://www.github.com/GoroXabi"><img src="https://avatars.githubusercontent.com/u/98422655?v=4"></a></th>
  </tr>
  <tr align="center">
    <td><a href="https://www.github.com/princess-mikus"> princess-mikus</a></td>
    <td><a href="https://www.github.com/GoroXabi"> GoroXabi</a> </td>
  </tr>
