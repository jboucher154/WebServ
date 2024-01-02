import sys
import os
from datetime import datetime , timezone
from email.utils import formatdate


html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>About Us</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            background-color: #f4f4f4;
            color: #333;
        }

		#navBar {
            list-style-type: none;
            margin: 0;
            padding: 0;
            display: flex;
        }
        
        #navBar li {
            margin-right: 20px;
        }
        
        #navBar a {
            text-decoration: none;
            color: white;
        }
		
        header {
            background-color: #333;
            color: white;
            padding: 10px;
        }

        section {
		    padding: 20px;
            margin: 20px 0;
        }

        h1 {
            color: #333;
        }

        p {
            line-height: 1.5;
        }
    </style>
</head>
<body>
    <header>
        <nav>
            <ul id="navBar">
                <li><a href="../index.html">Home</a></li>
                <li><a href="../cat/cats.html">Cat</a></li>
				<li><a href="../dog/index.html">Dog</a></li>
                <li><a href="../add_post/form.html">Add your post</a></li>
                <li><a href="../../cgi-bin/about_us.py">About us</a></li>
            </ul>
        </nav>
    </header>
    <section>
        <h2>Our Mission</h2>
		<p>We are the software engineers who have designed and built the webserver, serving this website.</p>
        <p>We have also designed and built this website.</p>
    </section>
    <section>
        <h2>Our Team</h2>
        <p>We are :</p>
		<ul>
		    <li><a href="https://github.com/azarSarikhani">Azzar Sarikhani</a> : software engineering student at Hive Helsinki</li>
		    <li><a href="https://github.com/jboucher154">Jenny Boucher</a> : software engineering student at Hive Helsinki</li>
		    <li><a href="https://github.com/sakarisalmi">Sakari Salmi</a> : software engineering student at Hive Helsinki</li>
		<ul>
    </section>
    <section>
        <h2>Contact Us</h2>
		<p>We are more than willing to chat with you about this project or answare questions you might have. Find us on linkedIn: </p>
        <ul>
		    <li><a href="https://fi.linkedin.com/in/azzar-sarikhani-07a912117">Azzar Sarikhani</a></li>
		    <li><a href="https://fi.linkedin.com/in/jenny-boucher-technurse?trk=public_profile_browsemap-profile">Jenny Boucher</a></li>
		    <li><a href="https://fi.linkedin.com/in/sakari-salmi-6aa574185?trk=public_profile_browsemap-profile">Sakari Salmi</a></li>
		<ul>
    </section>
</body>
</html>\r\n\r\n
"""
current_datetime = datetime.now(timezone.utc)
formatted_date = formatdate(timeval=current_datetime.timestamp(), localtime=False, usegmt=True)
http_version = os.environ.get('SERVER_PROTOCOL')
header=f"{http_version} 200 OK\r\nDate: {formatted_date}\r\nContent-Length: {len(html_content)}\r\nContent-Location: /cgi-bin\r\nContent-Type: text/html\r\n\r\n"

sys.stdout.write(header + html_content)

