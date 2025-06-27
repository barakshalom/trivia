#include "JsonRequestPacketDeserializer.h"


// All deserializer functions follow the same pattern:
// 1. Convert the Buffer (raw bytes) to a JSON string.
// 2. Parse the string into a JSON object.
// 3. Extract values from the JSON and return the corresponding request struct.

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const Buffer& buffer)
{
    // Convert buffer (raw bytes) to a JSON string.
    std::string jsonStr(buffer.begin(), buffer.end());

    // Convert buffer (raw bytes) to a JSON string.
    json jsonContainer = json::parse(jsonStr);

    // Extract username and password and return a LoginRequest object.
    return LoginRequest{ jsonContainer["username"].get<std::string>(),
                          jsonContainer["password"].get<std::string>() };
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return SignupRequest{ jsonContainer["username"].get<std::string>(),
                          jsonContainer["password"].get<std::string>(),
                          jsonContainer["email"].get<std::string>(),
                          jsonContainer["address"].get<std::string>(),
                          jsonContainer["phoneNumber"].get<std::string>(),
                          jsonContainer["birthDate"].get<std::string>() };

}

GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersInRoomRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return GetPlayersInRoomRequest{ jsonContainer["roomId"].get<unsigned int>() };
}

JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return JoinRoomRequest{ jsonContainer["roomId"].get<unsigned int>() };
}

CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return CreateRoomRequest{ jsonContainer["roomName"].get<std::string>(),
                              (unsigned)std::stoi(jsonContainer["maxUsers"].get<std::string>()),
                              (unsigned)std::stoi(jsonContainer["questionCount"].get<std::string>()),
                              (unsigned)std::stoi(jsonContainer["answerTimeout"].get<std::string>()) };
}

StatsRequest JsonRequestPacketDeserializer::deserializeStatsRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return StatsRequest{ jsonContainer["username"].get<std::string>() };
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializerSubmitAnswerRequest(const Buffer& buffer)
{
    std::string jsonStr(buffer.begin(), buffer.end());

    json jsonContainer = json::parse(jsonStr);

    return SubmitAnswerRequest{jsonContainer["answerId"].get<unsigned int>()};
}
