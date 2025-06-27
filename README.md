# Trivia Game Server & Client
This project is a multi client trivia game implemented with socket programming and database integration.

## Running the project
To run multiple clients simultaneously:
- The server should be running before starting the clients.
- Launch multiple instances using F5 in Visual Studio.

## Predefined users
You can log in with the following users (already in the database):
Username | password
'barak'  | 'Baraksh123!'
'saban'  | 'Saban123!'

## Signup validation rules
All fields must pass strict validation to create a new user.
- Password: 8+ characters, must include - at last 1 uppercase, 1 lowercase, 1 digit, 1 special character (!@#$%^&*)
- Email: must be in valid format - 'example@domain.com', must be uniqe (not already registered)
- Phone numebr: must start with '05' ('050', '053'...), must contain exacly 10 digits ('0523354056')
- Address: 'Street, Apt, City', street and city should be alphabetic, apt shoulb be numberic ('Herzl, 5, Tel Aviv')

## Technologies used
- C++
- SQLite
- WinSock2 (Windows only)
- WPF (for client GUI)

Created by: *Barak Shalom* and *Omer Saban*
Enjoy ;)
