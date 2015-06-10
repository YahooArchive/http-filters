/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef ATS_FILTERS
#define ATS_FILTERS

#include <sstream>
#include <ts/ts.h>

#include "compiler.h"
#include "representation.h"
#include "ts-impl.h"
#include "vm-impl.h"
#include "vm-printer.h"

#ifndef PLUGIN_TAG
#error Please define a PLUGIN_TAG before including this file.
#endif

struct Data {
  const http::filters::Code code;
  const http::filters::Memory memory;
  const http::filters::Offsets offsets;

  ~Data() {
    delete code.t;
    delete memory.t;
  }

  Data(const http::filters::Compiler & c,
      const http::filters::Offsets & o) :
    code(http::filters::Code::Copy(c.assembler_.code())),
    memory(http::filters::Memory::Copy(c.assembler_.memory())),
    offsets(o) { }
};

static int handler(TSCont continuation, TSEvent event, void * data) {
  TSHttpTxn transaction = static_cast< TSHttpTxn >(data);
  TSMBuffer buffer;
  TSMLoc header;

  if (TSHttpTxnClientReqGet(transaction, &buffer, &header) == TS_SUCCESS) {
    {
      using namespace http::filters;

      Data * data = static_cast< Data * >(TSContDataGet(continuation));
      ASSERT(data != NULL);

      typedef VM< TSImplementation > MyVM;
      MyVM vm(TSImplementation(PLUGIN_TAG, buffer, header),
          data->code, data->memory);

      const char * names[] = {
        "http get", "firefox", "yahoo domain", "slash-search",
        "california", "city starts with san",
      };

      for (int i = 0; i < ARRAY_SIZE(names); ++i) {
        if (vm.run(data->offsets[i])) {
          /*
           * replace here with your own logic.
           */
          TSDebug(PLUGIN_TAG, "vm result says it is: %s", names[i]);
        }
      }
    }

    TSHandleMLocRelease(buffer, TS_NULL_MLOC, header);
  }

  TSHttpTxnReenable(transaction, TS_EVENT_HTTP_CONTINUE);
  return 0;
}

void TSPluginInit(int argc, const char * * argv) {
  TSPluginRegistrationInfo info;
  info.plugin_name = const_cast< char * >(PLUGIN_TAG);
  info.support_email = const_cast< char * >("person@domain.com");
  info.vendor_name = const_cast< char * >("My Company");

  TSCont continuation = TSContCreate(handler, NULL);
  ASSERT(continuation != NULL);

  {
    using namespace http::filters;
    Forest f;

    {
      Tree t;
      t.addAnd();
        CHILD_OP(t, "isMethod", "GET");
        OP(t, "isScheme", "http");
        t.parent();
      f.push_back(t);
    }

    {
      Tree t;
      t.addOr();
        t.addChildAnd();
          CHILD_OP(t, "existsHeader", "User-Agent");
          OP(t, "containsHeader", "User-Agent", "Firefox");
          t.parent();
        t.addAnd();
          CHILD_OP(t, "existsHeader", "user-agent");
          OP(t, "containsHeader", "user-agent", "Firefox");
          t.parent();
        t.parent();
      f.push_back(t);
    }

    {
      Tree t;
      OP(t, "containsDomain", ".yahoo.com");
      f.push_back(t);
    }

    {
      Tree t;
      OP(t, "equalPath", "search");
      f.push_back(t);
    }

    {
      Tree t;
      OP(t, "containsQueryParameter", "state", "california");
      f.push_back(t);
    }

    {
      Tree t;
      OP(t, "startsWithQueryParameter", "city", "san");
      f.push_back(t);
    }

    {
      http::filters::Offsets o;
      o.reserve(f.size());

      Compiler c;
      c.compile(f, o);

      cleanAll(f);

      {
        vm::Printer printer;
        std::stringstream ss;
        ss << "printing vm code" "\n";
        printer.print(c.assembler_.code(),
            c.assembler_.memory(), ss);
        ss << "\n";
        std::cout << ss.str() << std::endl;
      }

      const Offsets::const_iterator end = o.end();
      Offsets::const_iterator it = o.begin();

      TSContDataSet(continuation, new Data(c, o));
    }
  }

  TSHttpHookAdd(TS_HTTP_SEND_REQUEST_HDR_HOOK, continuation);
}

#endif //ATS_FILTERS
