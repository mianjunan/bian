import org.junit.jupiter.api.*;
import top.bufeiniao.yuban.util.callback.Callback;
import top.bufeiniao.yuban.util.callback.Listener;
import top.bufeiniao.yuban.util.callback.NoneCallback;
import top.bufeiniao.yuban.util.config.ConfigElement;
import top.bufeiniao.yuban.util.config.ConfigOperate;

@DisplayName("测试util提供的操作")
public class UtilTest {

    @AfterAll
    public static void cleanup() {

    }

    @Nested
    @DisplayName("配置操作测试")
    class ConfigTest{
        private static ConfigOperate configOperate;
        @BeforeAll
        public static void init() {
            configOperate=new ConfigOperate(UtilTest.class.getResourceAsStream("/config/config.yml"));
        }
        @Test
        @DisplayName("常规读取测试")
        void normalReadTest() {
            Assertions.assertAll("常规读取",
                    ()->Assertions.assertEquals(configOperate.getData("test1.test1").getAsInteger(),1),
                    ()->Assertions.assertEquals(configOperate.getData("test1.test2").getAsDouble(),1.23456789101112),
                    ()->Assertions.assertEquals(configOperate.getData("test1.test3").getAsString(),"123456789101112"),
                    ()->Assertions.assertEquals(configOperate.getData("test1.test4").getAsLong(),123456789101112L),
                    ()->Assertions.assertTrue(configOperate.getData("test1.test5").getAsBoolean()),
                    ()->Assertions.assertEquals(configOperate.getData("test2").getAsArray().get(0),"test1")
            );
        }

        @Test
        @DisplayName("异常测试")
        void abnormalTest() {
            Assertions.assertAll("异常",
                    ()->Assertions.assertNull(configOperate.getData("test").getAsString()),
                    ()->Assertions.assertThrows(NullPointerException.class,
                            ()->configOperate.getData("test").getAsByte()),
                    ()->Assertions.assertThrows(ClassCastException.class,
                            ()->configOperate.getData("test1.test3").getAsByte())
            );
        }

        @Test
        @DisplayName("更新读取测试")
        void updateReadTest() {
            configOperate.putData("test1.test6",new ConfigElement<>(2));
            Assertions.assertAll("更新读取",
                    ()->Assertions.assertEquals(configOperate.getData("test1.test6").getAsInteger(),2)
            );
        }
    }

    @Nested
    @DisplayName("回调函数测试")
    class CallbackTest{

        @Nested
        @TestMethodOrder(MethodOrderer.OrderAnnotation.class)
        @DisplayName("有参数回调函数测试")
        class ArgsTest{
            private static int callbackTestData1=0;
            private static int callbackTestData2=0;
            private static Callback<Integer, Listener<Integer>> callback;
            @BeforeAll
            public static void init() {
                callback=new Callback<>();
                callback.addListener((i)->callbackTestData1=i);
                callback.addListener((i)->callbackTestData2=i);
            }

            @Test
            @Order(1)
            @DisplayName("常规执行测试")
            void normalReadTest() {
                Assertions.assertAll("常规执行",
                        ()->{
                            callback.run(1);
                            Assertions.assertEquals(callbackTestData1,1);
                            Assertions.assertEquals(callbackTestData2,1);
                        }
                );
            }

            @Test
            @Order(2)
            @DisplayName("移除执行测试")
            void removeReadTest() {
                Assertions.assertAll("移除执行",
                        ()->{
                            callback.removeListener(1);
                            callback.run(2);
                            Assertions.assertEquals(callbackTestData1,2);
                            Assertions.assertEquals(callbackTestData2,1);
                        }
                );
            }
            @Test
            @Order(3)
            @DisplayName("异常测试")
            void exceptionTest() {
                Assertions.assertAll("异常",
                        ()->Assertions.assertThrows(
                                IndexOutOfBoundsException.class,
                                ()->callback.removeListener(1))
                );
            }
        }

        @Nested
        @TestMethodOrder(MethodOrderer.OrderAnnotation.class)
        @DisplayName("无参数回调函数测试")
        class NoneArgsTest{
            private static int callbackTestData1=0;
            private static int callbackTestData2=0;
            private static NoneCallback noneCallback;
            @BeforeAll
            public static void init() {
                noneCallback=new NoneCallback();
                noneCallback.addListener(()->callbackTestData1+=1);
                noneCallback.addListener(()->callbackTestData2+=1);
            }

            @Test
            @Order(1)
            @DisplayName("常规执行测试")
            void normalReadTest() {
                Assertions.assertAll("常规执行",
                        ()->{
                            noneCallback.run();
                            Assertions.assertEquals(callbackTestData1,1);
                            Assertions.assertEquals(callbackTestData2,1);
                        }
                );
            }

            @Test
            @Order(2)
            @DisplayName("移除执行测试")
            void removeReadTest() {
                Assertions.assertAll("移除执行",
                        ()->{
                            noneCallback.removeListener(1);
                            noneCallback.run();
                            Assertions.assertEquals(callbackTestData1,2);
                            Assertions.assertEquals(callbackTestData2,1);
                        }
                );
            }
            @Test
            @Order(3)
            @DisplayName("异常测试")
            void exceptionTest() {
                Assertions.assertAll("异常",
                        ()->Assertions.assertThrows(
                                IndexOutOfBoundsException.class,
                                ()->noneCallback.removeListener(1))
                );
            }
        }

    }



}
