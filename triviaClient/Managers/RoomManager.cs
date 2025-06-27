using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using trivia_gui.Pages;

namespace trivia_gui.Managers
{
    public static class RoomManager
    {
        // Dictionary: room name -> RoomPage instance
        private static Dictionary<string, RoomPage> _rooms = new Dictionary<string, RoomPage>();

        public static void AddRoom(string roomName,RoomPage room)
        {
            if (!_rooms.ContainsKey(roomName))
                _rooms[roomName] = room;
                
        }

        public static RoomPage? GetRoom(string roomName)
        {
            return _rooms.TryGetValue(roomName, out var room) ? room : null;
        }

        public static bool RoomExists(string roomName)
        {
            return _rooms.ContainsKey(roomName);
        }
    }
}
