using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using trivia_gui.Pages;

namespace trivia_gui
{
    // Enumeration of every request opcode exchanged with the server.
    // Each value identifies a different API call in the trivia protocol.
    enum Requests
    {
        NONE = 0x0, ERROR_MSG, LOGIN, SIGNUP, CREATE_ROOM, JOIN_ROOM, GET_PLAYERS_IN_ROOM, STATS, LOGOUT, GET_ROOMS, CLOSE_ROOM, START_ROOM,
        STATE, GET_HIGH, LEAVE_ROOM, LEAVE_GAME, GET_QUESTION, SUBMIT_ANSWER, GAME_RESULTS, EXIT
    }

    // Encapsulates all client side socket operations.
    // Connects to the trivia server, serializes requests as json, 
    // and parses responses back into usable objects.
    public class ServerSocket
    {
        private const int TRUE = 1;
        private const int FALSE = 0;

        // All packets sent by server start with a 5-byte header:
        // [0] return code, [1-4] payload length.
        private const int START = 5;

        // Size of temporary buffer for single read
        private const int BUFFER = 1024;

        // Endpoint details (localhost:10000)
        private const int PORT = 10000;
        private const string HOST = "127.0.0.1";

        private Socket _serverSocket; // Active TCP socket
        private string _currentUsername; // Currently logged in user

        private readonly object _sendReceiveLock = new object(); // its like a mutex in c#

        // Initializes the socket and connects immediately.
        public ServerSocket()
        {
            IPAddress[] IPs = Dns.GetHostAddresses(HOST);
            _serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _serverSocket.Connect(IPs[0], PORT);
            _currentUsername = "";

        }

        // Gracefully logs out (if needed) and shuts down the socket.
        public void Close()
        {
            if (_serverSocket != null && _serverSocket.Connected)
            {
                if (_currentUsername != null)
                {
                    Logout();
                }
                _serverSocket.Shutdown(SocketShutdown.Both);
                _serverSocket.Close();
            }
        }

        // Thread-safe helper: send a request and wait for raw json response.
        public string SendAndReceive(string jsonBuffer, byte code)
        {
            lock (_sendReceiveLock)
            {

                if (_serverSocket != null && _serverSocket.Connected)
                {
                    Send(jsonBuffer, code);
                    return Receive();
                }
            }
            return string.Empty;
        }

        // Read bytes off the socket and strip the 5-byte header.  
        public string Receive()
        {
            byte[] recvBuffer = new byte[BUFFER];
            int bytesReceived = _serverSocket.Receive(recvBuffer);
            return Encoding.UTF8.GetString(recvBuffer, START, bytesReceived - START);
        }

        // Same as SendAndReceive but returns the first header byte (status/return code) too.
        public (int code, string message) SendReceiveWithCode(string jsonBuffer, byte code)
        {
            lock (_sendReceiveLock)
            {

                if (_serverSocket != null && _serverSocket.Connected)
                {
                    Send(jsonBuffer, code);
                    byte[] recvBuffer = new byte[BUFFER];
                    int bytesReceived = _serverSocket.Receive(recvBuffer);
                    int returnCode = recvBuffer[0];
                    return (returnCode, Encoding.UTF8.GetString(recvBuffer, 5, bytesReceived - 5));
                }
            }
            return (0, string.Empty);
        }

        // Compose the packet: [code][length(int32)][jsonBytes] and transmit.
        public void Send(string jsonBuffer, byte code)
        {
            using MemoryStream ms = new MemoryStream();
            using BinaryWriter writer = new BinaryWriter(ms);
            byte[] jsonBytes = Encoding.UTF8.GetBytes(jsonBuffer);
            writer.Write(code);             // 1 byte opcode
            writer.Write(jsonBytes.Length); // 4 byte length
            writer.Write(jsonBytes);        // payload
            _serverSocket.Send(ms.ToArray());
        }

        // Getters and setters 
        public void setUsername(string username)
        {
            this._currentUsername = username;
        }

        public string getUsername()
        {
            return _currentUsername;
        }

        // Sends logout request, clears username.
        public void Logout()
        {
            SendAndReceive(string.Empty, (byte)Requests.LOGOUT);
            setUsername(string.Empty);
        }

        // Sends closeroom request.
        public void CloseRoom()
        {
            SendAndReceive(string.Empty, (byte)Requests.CLOSE_ROOM);
        }

        // Sends signup request.
        public bool Signup(string username, string password, string email, string address, string phoneNumber, string birthDate)
        {
            string jsonString = JsonSerializer.Serialize(new { username, password, email, address, phoneNumber, birthDate });
            var result = JsonSerializer.Deserialize<Dictionary<string, int>>(SendAndReceive(jsonString, (byte)Requests.SIGNUP));
            if (result?["status"] == TRUE) { return true; }

            return false;
        }

        // Sends login request.
        public bool Login(string username, string password)
        {
            string jsonString = JsonSerializer.Serialize(new { username, password });
            var result = JsonSerializer.Deserialize<Dictionary<string, int>>(SendAndReceive(jsonString, (byte)Requests.LOGIN));
            if (result?["status"] == TRUE) { return true; }

            return false;
        }

        // Sends creategame request.
        public bool CreateRoom(string roomName, string maxUsers, string questionCount, string answerTimeout)
        {
            string jsonString = JsonSerializer.Serialize(new { roomName, maxUsers, questionCount, answerTimeout });
            var result = JsonSerializer.Deserialize<Dictionary<string, int>>(SendAndReceive(jsonString, (byte)Requests.CREATE_ROOM));
            if (result?["status"] == TRUE) { return true; }

            return false;
        }

        public (string, string, string, string) Stats()
        {
            string username = _currentUsername;
            string jsonBuffer = JsonSerializer.Serialize(new { username });
            var result = JsonSerializer.Deserialize<Dictionary<string, object>>(SendAndReceive(jsonBuffer, (byte)Requests.STATS));
            var stats = JsonSerializer.Deserialize<Dictionary<string, object>>(result?["Stats"].ToString());
            var totalGames = stats?["games"].ToString();
            var correctAnswers = stats?["correct"].ToString();
            var totalAnswers = stats?["total"].ToString();
            var timeAnswer = stats?["time"].ToString();

            return (totalGames, correctAnswers, totalAnswers, timeAnswer);
        }

        // Sends high scores request.
        public List<string> Scores()
        {
            var result = JsonSerializer.Deserialize<Dictionary<string, List<string>>>(SendAndReceive(string.Empty, (byte)Requests.GET_HIGH));
            return result["Scores"];
        }

        // Sends get players request.
        public Dictionary<string, List<string>> getPlayers(object roomId)
        {
            string jsonString = JsonSerializer.Serialize(new { roomId });
            var players = JsonSerializer.Deserialize<Dictionary<string, List<string>>>(SendAndReceive(jsonString, (byte)Requests.GET_PLAYERS_IN_ROOM));

            return players;
        }

        // Sends join room request.
        public bool joinRoom(int roomId)
        {
            string jsonString = JsonSerializer.Serialize(new { roomId });
            var status = JsonSerializer.Deserialize<Dictionary<string, int>>(SendAndReceive(jsonString, (byte)Requests.JOIN_ROOM));
            if (status?["status"] == TRUE) { return true; }

            return false;
        }

        // Sends get rooms request.
        public List<Dictionary<string, object>> getRooms()
        {
            try
            {
                var result = JsonSerializer.Deserialize<Dictionary<string, object>>(SendAndReceive(string.Empty, (byte)Requests.GET_ROOMS));
                var rooms = JsonSerializer.Deserialize<List<Dictionary<string, object>>>(result["Rooms"].ToString());
                return rooms;
            }
            catch
            {
                throw new Exception();
            }

        }

        // Sends ger results request.
        public JsonElement getResults()
        {
            var resultsObj = JsonSerializer.Deserialize<Dictionary<string, JsonElement>>(SendAndReceive(string.Empty, (byte)Requests.GAME_RESULTS));
            // obj["Results"] is an array: [overall, currentPlayer]
            var playerResults = resultsObj["Results"][1];
            return playerResults;
        }

        // Sends leave game request.
        public void leaveGame()
        {
            SendAndReceive(string.Empty, (byte)Requests.LEAVE_GAME);
        }

        // Sends submit answer request.
        public bool submitAnswer(int answerId)
        {
            var id = JsonSerializer.Serialize(new { answerId });
            var answerObj = JsonSerializer.Deserialize<Dictionary<string, JsonElement>>(SendAndReceive(id, (byte)Requests.SUBMIT_ANSWER));
            var answer = answerObj["Submit"];
            if (answer.GetProperty("status").GetInt32() == TRUE) { return true; }

            return false;
        }

        // Sends get question request.
        public (bool, JsonElement) getQuestion()
        {
            var questionObj = JsonSerializer.Deserialize<Dictionary<string, JsonElement>>(SendAndReceive(string.Empty, (byte)Requests.GET_QUESTION));
            var question = questionObj["Questions"];
            // q[0][1] == status flag: 1 if question exists, 0 if game ended.
            return (question[0][1].GetInt32() == TRUE, question);

        }

        // Sends get state request.
        public (int code, string message) getState()
        {
            return SendReceiveWithCode(string.Empty, (byte)Requests.STATE);
        }

        // Sends start game request.
        public void StartGame()
        { 
            SendReceiveWithCode(string.Empty, 0xB); // 0xB == START_ROOM
        }

    }
}
