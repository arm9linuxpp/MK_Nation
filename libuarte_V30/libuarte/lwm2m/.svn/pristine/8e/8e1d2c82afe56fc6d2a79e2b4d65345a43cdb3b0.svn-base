to debug the linux command line use.


diff --git a/examples/client/CMakeLists.txt b/examples/client/CMakeLists.txt
index aae1dd2..1d66bef 100644
--- a/examples/client/CMakeLists.txt
+++ b/examples/client/CMakeLists.txt
@@ -21,13 +21,13 @@ set(SOURCES

 # Client without DTLS support
 add_executable(lwm2mclient ${SOURCES})
-target_compile_definitions(lwm2mclient PRIVATE LWM2M_CLIENT_MODE LWM2M_BOOTSTRAP LWM2M_SUPPORT_SENML_JSON)
+target_compile_definitions(lwm2mclient PRIVATE LWM2M_CLIENT_MODE LWM2M_BOOTSTRAP LWM2M_SUPPORT_SENML_JSON   )
 target_sources_wakaama(lwm2mclient)
 target_sources_shared(lwm2mclient)

 # Client with DTLS support provided by tinydtls
 add_executable(lwm2mclient_tinydtls ${SOURCES})
 set_target_properties(lwm2mclient_tinydtls PROPERTIES CONNECTION_IMPLEMENTATION "tinydtls")
-target_compile_definitions(lwm2mclient_tinydtls PRIVATE LWM2M_CLIENT_MODE LWM2M_BOOTSTRAP LWM2M_SUPPORT_SENML_JSON)
+target_compile_definitions(lwm2mclient_tinydtls PRIVATE LWM2M_CLIENT_MODE LWM2M_BOOTSTRAP LWM2M_SUPPORT_SENML_JSON HAVE_VPRINTF LWM2M_WITH_LOGS  )
 target_sources_wakaama(lwm2mclient_tinydtls)
 target_sources_shared(lwm2mclient_tinydtls)
mza@dg8ser-c:~/work/git/wakaama$

qtWakaClient\tinydtls\dtls_debug.c
static int maxlog = DTLS_LOG_DEBUG; // DTLS_LOG_WARN;  

dtls_logging_handler -> 
if (print_timestamp(timebuf,sizeof(timebuf), time(NULL)))
    fprintf(log_fd, "%s ", timebuf);
	
