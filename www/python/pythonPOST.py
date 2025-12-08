#!/usr/bin/python3

import os
import sys
import cgi

print("Content-Type: text/html\n")
print("<html>")
print("<head><title>Python POST - Create Resource</title></head>")
print("<body>")
print("<h1>POST Request Processed by Python CGI</h1>")

content_length = int(os.environ.get("CONTENT_LENGTH", 0))

if content_length > 0:
    post_data = sys.stdin.read(content_length)

    import datetime
    timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
    file_name = f"resource_{timestamp}.txt"
    
    upload_dir = "../../www/uploads"
    
    if not os.path.exists(upload_dir):
        try:
            os.makedirs(upload_dir)
        except OSError as e:
            print(f"<p style='color:red;'>Error creating directory: {e}</p>")
            print("</body></html>")
            sys.exit(1)
            
    file_path = os.path.join(upload_dir, file_name)

    try:
        with open(file_path, "w") as f:
            f.write(post_data)
        print(f"<p>Resource created successfully: <strong>{file_name}</strong></p>")
        print(f"<p>Content saved in: <code>{file_path}</code></p>")
    except Exception as e:
        print(f"<p style='color:red;'>Error saving resource: {e}</p>")
    
    print("<p>POST data recived:</p>")
    print(f"<pre>{post_data}</pre>")
else:
    print("<p>POST data missing to create a resource.</p>")

print("</body>")
print("</html>")

#curl -X POST -H "Content-Type: text/plain" --data "Este es el contenido de mi nuevo recurso." http://localhost:8081/cgi-bin/pythonPOST.py