// Copyright (c) 2025-2026 Michael Roe
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

import { McpServer, ResourceTemplate }
  from "@modelcontextprotocol/sdk/server/mcp.js";

import { StdioServerTransport }
  from "@modelcontextprotocol/sdk/server/stdio.js";

import { z }
  from "zod";

//
// Weather forecast MCP server
//
// The weather forecast is often used as an example in tutorials for
// how to do function calling from an LLM.
//

// Create an MCP server

const server = new McpServer({
  name: "weather",
  version: "1.0.0"
});

const cambridge = "<listPlace><place><placeName>Cambridge</placeName><location><country>UK</country></location><location><geo>52.205 0.1225</geo></location></place></listPlace>";


//
// Tools
//

//
// Deep Research MCP clients expect the server to provide tools called
// "search" and "fetch".
//

//
// Fetch
//
// The LLM is expecting the result to be: {name, title, text, uri}
//
// If we return a resource, rather than a text block containing the JSON
// the LLM is expecting, the glue logic between the LLM and MCP is
// responsible for turning the fields in the resource into the above form.
//
//

server.registerTool("fetch",
  {
    title: "Fetch",
    description: "Retrieve a document from the Vector Store by ID",
    inputSchema: { document_id: z.string() }
  },
  async ({ document_id }) => ({
    content: [{
      type: "resource", 
      resource: {
        name: "gazeteer-uk-cambridge",
        title: "Cambridge, UK",
        mimeType: "text/xml",
        text: cambridge,
        uri: "file:///Gazeteer/UK/Cambridge"
      }
    }]
  })
);

//
// Search Gazeteer
//
// Returns a list of {id, title, text, uri}
// text is a snippet, not the whole document
//
//

server.registerTool("search",
  {
    title: "Search Gazeteer",
    description: "Search the gazeteer for matching documents",
    inputSchema: { placename: z.string() }
  },
  async ({ placename }) => ({
    content: [{ type: "text", text: `[{\"id\":\"cambridge\", \"title\":\"Cambridge, UK\", \"text\":${cambridge}, \"uri\":\"file:///Gazeteer/UK/Cambridge\"}]` }]
  })
);

server.registerTool("search_weather",
  {
    title: "Search Weather",
    description: "Search weather reports for matching documents",
    inputSchema: { latitude: z.string(), longitude: z.string() }
  },
  async ({ latitude, longitude }) => {
    const endpoint = `https://api.open-meteo.com/v1/forecast?latitude=${latitude}&longitude=${longitude}&current=temperature_2m,relative_humidity_2m,wind_speed_10m`;

    const response = await fetch(endpoint);

    const result = await response.json();

    return {
      content: [{ type: "text", text: `<p>Weather report for <geo>${result.latitude} ${result.longitude}</geo> on <date>${result.current.time}</date></p><p><measure commodity="temperature"><num>${result.current.temperature_2m}</num><unit>${result.current_units.temperature_2m}</unit></measure><measure commodity="humidity"><num>${result.current.relative_humidity_2m}</num><unit>${result.current_units.relative_humidity_2m}</unit></measure></p>` }]
    }
  }
);

//
// Query with location
//

server.registerPrompt(
  "query_with_location",
  {
    title: "Query with Location",
    description: "Attach latitude and longitude to query",
    argsSchema: { location: z.string() }
  },
  ({ location }) => ({
    messages: [{
      role: "user",
      content: {
        type: "text",
        text: "What's the weather in Cambridge?"
      }}, {
      role: "user",
      content: {
        type: "resource",
        resource: {
          uri: "file:///Gazeteer/UK/Cambridge",
          name: "gazeteer-uk-cambridge",
          title: "Cambridge, UK",
          mimeType: "text/xml",
          text: cambridge,
      }}
    }]
  })
);

//
// Resources
//

server.registerResource(
  "gazeteer-uk-cambridge",
  "file:///Gazeteer/UK/Cambridge",
  {
    title: "Cambridge, UK",
    description: "Gazeteer entry for Cambridge, UK",
    mimeType: "text/xml",
  },
  async (uri) => ({
    contents: [{
      uri: uri.href,
      text: cambridge
    }]
  })
);


// Start receiving messages on stdin and sending messages on stdout
const transport = new StdioServerTransport();
server.connect(transport);
