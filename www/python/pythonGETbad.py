#!/usr/bin/python3

import os
import datetime

# --- Obtener la fecha y hora actual ---
current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S %Z")

# --- Lógica CGI ---
query_string = os.environ.get("QUERY_STRING", "")
params_html = ""

if query_string:
    params_list = query_string.split('&')
    params_html += "<ul>"
    for param in params_list:
        if '=' in param:
            key, value = param.split('=', 1)
            params_html += f"<li><strong>{key}:</strong> {value}</li>"
        else:
            params_html += f"<li>{param}</li>"
    params_html += "</ul>"
else:
    params_html = "<p>No se proporcionó un Query String.</p>"

while True:
	pass

# --- Imprimir la respuesta HTML ---
print("Content-Type: text/html\n")
print(f"""
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Python GET</title>
    <style>
        body {{ font-family: Arial, sans-serif; background-color: #f4f4f9; color: #333; margin: 0; padding: 20px; }}
        .container {{ max-width: 800px; margin: auto; background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        h1 {{ color: #0056b3; }}
        .timestamp {{ background: #e7f3ff; border-left: 5px solid #0056b3; padding: 10px; margin-bottom: 20px; }}
        .data-box {{ background: #fafafa; border: 1px solid #ddd; padding: 15px; margin-top: 20px; border-radius: 5px; }}
        ul {{ list-style-type: none; padding: 0; }}
        li {{ background: #eee; margin-bottom: 5px; padding: 8px; border-radius: 3px; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>GET Request Procesada por Python 🐍</h1>
        <div class="timestamp">
            <strong>Hora de Ejecución:</strong> {current_time}
        </div>
        <div class="data-box">
            <h2>Datos Recibidos:</h2>
            {params_html}
        </div>
    </div>
</body>
</html>
""")