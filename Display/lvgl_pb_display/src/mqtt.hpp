#pragma once

#include <chrono>
#include <string>
#include <mqtt/async_client.h>

#include "config.h"

class Mqtt {
    static const int MAX_BUFFERED_MSGS = 500e3; // with around 500 bytes/msg, buffer up to 250MB
    mqtt::async_client* client;

  public:
    Mqtt() {
        printf("MQTT connecting... ");
        fflush(stdout);

        auto createOpts = mqtt::create_options_builder()
            .server_uri(MQTT_SERVER)
            .send_while_disconnected(true, true)
            .max_buffered_messages(MAX_BUFFERED_MSGS)
            .delete_oldest_messages()
            .persist_qos0(true)
            .finalize();
        client = new mqtt::async_client(createOpts);


        // auto sslopts = mqtt::ssl_options_builder()
        //                    .trust_store(TRUST_STORE)
        //                    .enable_server_cert_auth(false)
        //                    .finalize();

        // TODO: Last will?
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        connOpts.set_automatic_reconnect(5, 30);
        connOpts.set_user_name(MQTT_USER);
        connOpts.set_password(MQTT_PASS);
        connOpts.set_max_inflight(16);  // default is 64k, leading to massive slowdowns at around 14k queued messages
        // connOpts.set_ssl(sslopts);

        client->connect(connOpts);

        printf("done.\n");
    }

    void publishJson(JsonDocument doc, bool retained = true) {
        const char *system = doc["system"];
        const char *type = doc["type"];

        if (system && type) {
            doc["timestamp"] = Util::getCurrentTimestamp();

            std::string topic = "pocketbike/";
            topic.append(system).append("/").append(type);

            std::string payload;
            serializeJson(doc, payload);

            mqtt::message_ptr msg = mqtt::make_message(topic, payload.c_str());
            msg->set_qos(0);
            msg->set_retained(retained);
            
            client->publish(msg);
        }
    }

    size_t getQueued() {
        return client->get_buffered_messages_num();
    }
};