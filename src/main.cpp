#include <memory>
#include "Actor.h"
#include "Callbacks.h"

#include "Util.h"
#include "Parser.h"
#include "Reporter.h"
#include "Timing.h"
#include "Cache.h"
#include "ProcessingRule.h"
#include "InvalidRequest.h"
#include "InvalidHttpMethod.h"
#include "RequestRecorder.h"

int main(int argc, char **argv)
{
    auto path = "/usr/local/nginx/logs";
    Actors actors;

    TIME(Processing)

    Util::process_files(
            Util::collect_files(path),
            Callbacks::add_request(actors),
            Callbacks::add_invalid_request(actors)
    );

    Cache cache("localhost", 6379);
    cache.flush_requests();

    std::set<std::shared_ptr<ProcessingRule>> rules = {
            std::make_shared<InvalidRequest>(InvalidRequest()),
            std::make_shared<RequestRecorder>(RequestRecorder()),
            std::make_shared<InvalidHttpMethod>(InvalidHttpMethod(std::set<std::string>{"GET", "POST"}))
    };

    Reporter::write(cache, actors, rules);
}
