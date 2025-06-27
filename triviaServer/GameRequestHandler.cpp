#include "GameRequestHandler.h"
#include "MenuRequestHandler.h"

GameRequestHandler::GameRequestHandler(RequestHandlerFactory* factory, std::shared_ptr<LoggedUser> user, std::shared_ptr<Game> game){
    m_handlerFactory = factory;
    m_user = user;
    m_game = game;
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& request) const {
    return request.id == RequestId::LEAVE_GAME || request.id == RequestId::GET_QUESTION || request.id == RequestId::SUBMIT_ANSWER || request.id == RequestId::GAME_RESULTS; // Checking if request is
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& request) const {
    RequestResult result;
    std::shared_ptr<LoggedUser> user;
    std::shared_ptr<Game> game;
    ErrorResponse errorResponse;

    // Making a temp pointer 
    if (isRequestRelevant(request))
    {
        std::map<RequestId, void(GameRequestHandler::*)(RequestResult&, const Buffer&)const> handlers = {
        {RequestId::LEAVE_GAME,&GameRequestHandler::leaveGame},
        {RequestId::GET_QUESTION,&GameRequestHandler::getQuestion},
        {RequestId::SUBMIT_ANSWER,&GameRequestHandler::submitAnswer},
        {RequestId::GAME_RESULTS,&GameRequestHandler::getGameResults}
        };
        auto it = handlers.find(request.id);
        if (it != handlers.end())
        {
            auto func = it->second;
            (this->*func)(result , request.buffer);
        }
    }
    else {
        // request exception
        result.newHandler = std::move(m_handlerFactory->createGameRequestHandler(m_user, m_game));
        errorResponse.message = "Error: Not A Game Request!";
        result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
    }

    return result;
}

void GameRequestHandler::leaveGame(RequestResult& result, const Buffer& buffer) const {
    LeaveGameResponse leaveGameResponse;
    leaveGameResponse.status = 1;
    for (auto result : m_game->getPlayerResults())
    {
        if (result.username == m_user->getUsername())
        {
            m_handlerFactory->getGameManager()->submitGameStatsToDB(result);
        }
    }
    m_game->removePlayer(m_user);
    if (m_game->getPlayerCount())
    {
        m_handlerFactory->getGameManager()->deleteGame(m_game->getGameId());
    }
    result.response = JsonResponsePacketSerializer::serializeResponse(leaveGameResponse);
    result.newHandler = std::move(m_handlerFactory->createMenuRequestHandler(m_user));
}

void GameRequestHandler::getQuestion(RequestResult& result, const Buffer& buffer) const {
    GetQuestionResponse getQuestionResponse;
    
    Question q = m_game->getQuestionForUser(m_user);
    getQuestionResponse.question = q.getQuestion();
    for (int i = 0; i < q.getPossibleAnswers().size(); i++)
    {
        getQuestionResponse.answers[i] = q.getPossibleAnswers()[i];
    }
    getQuestionResponse.status = 1;
    result.response = JsonResponsePacketSerializer::serializeResponse(getQuestionResponse);
    result.newHandler = std::move(m_handlerFactory->createGameRequestHandler(m_user, m_game));

}

void GameRequestHandler::submitAnswer(RequestResult& result, const Buffer& buffer) const 
{
    SubmitAnswerResponse submitAnswerResponse;
    SubmitAnswerRequest answerRequest = JsonRequestPacketDeserializer::deserializerSubmitAnswerRequest(buffer);
    m_game->submitAnswer(m_user, answerRequest.answerId) ? submitAnswerResponse.status = 1 : submitAnswerResponse.status = 0;
    submitAnswerResponse.correctAnswerId = m_game->getQuestionForUser(m_user).getCorrectAnswerId();
    result.response = JsonResponsePacketSerializer::serializeResponse(submitAnswerResponse);
    result.newHandler = std::move(m_handlerFactory->createGameRequestHandler(m_user, m_game));
}

void GameRequestHandler::getGameResults(RequestResult& result, const Buffer& buffer) const {
    GetGameResultsResponse getGameResultsResponse;
    getGameResultsResponse.results = m_game->getPlayerResults();
    result.response = JsonResponsePacketSerializer::serializeResponse(getGameResultsResponse);
    result.newHandler = std::move(m_handlerFactory->createGameRequestHandler(m_user, m_game));
}

