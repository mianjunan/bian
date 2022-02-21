package top.bufeiniao.yuban.service.web;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import top.bufeiniao.yuban.util.callback.Callback;
import top.bufeiniao.yuban.util.callback.Listener;
import top.bufeiniao.yuban.util.pipeline.Pipeline;
import top.bufeiniao.yuban.util.pipeline.PipelineTask;


public class MessageAccept extends TextWebSocketHandler {
    private static final Logger log = LoggerFactory.getLogger(MessageAccept.class);

    private static final Gson gson = new GsonBuilder().create();

    private static final Callback<Data, Listener<Data>> HANDLE_TEXT_MESSAGE =new Callback<>();

    private static final Callback<WebSocketSession, Listener<WebSocketSession>> AFTER_CONNECTION_ESTABLISHED =new Callback<>();

    private static final Callback<WebSocketSession, Listener<WebSocketSession>> AFTER_CONNECTION_CLOSED =new Callback<>();

    private static final Pipeline<Data> handleTextMessage =new Pipeline<>(new PipelineTask<>() {

        @Override
        public void preTreat(Data data) {
            data.handle();
        }

        @Override
        public void treat(Data data) {
            HANDLE_TEXT_MESSAGE.run(data);
        }

        @Override
        public void exception(Data data,Exception e) {
            if(data!=null){
                log.trace("websocket 消息处理出现异常 '{}': {}",data.getSession().getId() ,e.getMessage());
            }
        }
    });

    @Override
    public void handleTextMessage(WebSocketSession session, TextMessage message) {
        Data data= new Data();
        data.message=message;
        data.session=session;
        handleTextMessage.addData(data);
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) {
        AFTER_CONNECTION_CLOSED.run(session);
    }

    public static Callback<Data, Listener<Data>> getHandleTextMessage(){
        return HANDLE_TEXT_MESSAGE;
    }

    public static Callback<WebSocketSession, Listener<WebSocketSession>> getAfterConnectionEstablished(){
        return AFTER_CONNECTION_ESTABLISHED;
    }

    public static Callback<WebSocketSession, Listener<WebSocketSession>> getAfterConnectionClosed(){
        return AFTER_CONNECTION_CLOSED;
    }

    public static class Data{
        private TextMessage message;
        private JsonObject preMessage;
        private WebSocketSession session;
        private void handle(){
            preMessage=gson.fromJson(message.getPayload(), JsonObject.class);
            message=null;
            log.debug("websocket 收到消息 '{}': {}", session.getId(), preMessage);
        }
        public WebSocketSession getSession(){
            return session;
        }
        public JsonObject getPreMessage(){
            return preMessage;
        }
    }
}
