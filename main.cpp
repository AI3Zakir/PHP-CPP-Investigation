#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include <phpcpp.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;

#define NUM_THREADS 5

/**
 *  Bubblesort function
 *
 *  @param  array       An unsorted array
 *  @return array       A sorted array
 */
Php::Value native_bubblesort(Php::Parameters &params) {
    std::vector<int> input = params[0];

    for (size_t i = 0; i < input.size(); i++) {
        for (size_t j = 1; j < input.size() - i; j++) {
            if (input[j - 1] <= input[j]) continue;

            int temp = input[j];
            input[j] = input[j - 1];
            input[j - 1] = temp;
        }
    }

    return input;
}

/**
 * Thread function
 *
 * @param t
 * @return
 */
void *wait(void *t) {
    int i;
    long tid;

    tid = (long) t;

    usleep(100000);
    Php::out << "Sleeping in thread " << std::endl;
    Php::out << "Thread with id : " << tid << "  ...exiting " << std::endl;
    pthread_exit(NULL);
}

/**
 * Multithreading with PHP-CPP
 *
 * @return
 */
Php::Value multithreading() {
    int rc;
    int i;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < NUM_THREADS; i++) {
        Php::out << "multithreading() : creating thread, " << i << std::endl;
        rc = pthread_create(&threads[i], &attr, wait, (void *) i);

        if (rc) {
            Php::out << "Error:unable to create thread," << rc << std::endl;
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            Php::out << "Error:unable to join," << rc << std::endl;
            exit(-1);
        }

        Php::out << "multithreading():: completed thread id :" << i;
        Php::out << "  exiting with status :" << status << std::endl;
    }

    Php::out << "multithreading():: program exiting." << std::endl;

    return "Hello";
}

/**
 * Make request with POCO
 *
 * @param session
 * @param request
 * @param response
 * @return
 */
bool doRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);

    Php::out << response.getStatus() << " " << response.getReason() << std::endl;
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::string responseStr;
        StreamCopier::copyToString(rs, responseStr);

        Php::out << responseStr << std::endl;

        return true;
    }
    else
    {
        //it went wrong ?
        return false;
    }
}

/**
 * Make http request with PHP-CPP
 *
 * @return
 */
Php::Value make_request_with_cpp(Php::Parameters &params) {
    std::string input = params[0];

    Php::out << input << std::endl;

    URI uri(input);
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";
    HTTPClientSession session(uri.getHost(), uri.getPort());
    HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    HTTPResponse response;

    if (!doRequest(session, request, response))
    {
        std::cerr << "Invalid Request." << std::endl;
        return 1;
    }
}

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("printZakir", "1.0");
        extension.add<multithreading>("multithreading");
        extension.add<native_bubblesort>("native_bubblesort", {
                Php::ByVal("input", Php::Type::Array)
        });
        extension.add<make_request_with_cpp>("make_request_with_cpp", {
                Php::ByVal("input", Php::Type::String)
        });
        // return the extension
        return extension;
    }
}
