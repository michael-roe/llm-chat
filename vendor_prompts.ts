import { McpServer, ResourceTemplate }
  from "@modelcontextprotocol/sdk/server/mcp.js";

import { StdioServerTransport }
  from "@modelcontextprotocol/sdk/server/stdio.js";

import { z }
  from "zod";

//
// vendor_prompts
//
// An MCP server that returns the vendor's recommened system prompt for
// several popular large languages models.
//

// Create an MCP server

const server = new McpServer({
  name: "vendor-prompts",
  version: "1.0.0"
});

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
// Returns {document_id, title, text, url}
//
// TO DO: return this, not "Hello World!"
//

server.registerTool("fetch",
  {
    title: "Fetch",
    description: "Retrieve a document from the Vector Store by ID",
    inputSchema: { document_id: z.string() }
  },
  async ({ document_id }) => ({
    content: [{ type: "text", text: "Hello World!" }]
  })
);

//
// Search
//
// Returns a list of {id, title, text, uri}
// text is a snippet, not the whole document
//
// TO DO: return this, not [1]
//

server.registerTool("search",
  {
    title: "Search",
    description: "Search for matching documents in the Vector Store",
    inputSchema: { query: z.string() }
  },
  async ({ query }) => ({
    content: [{ type: "text", text: "[1]" }]
  })
);

//
// Get Weather
//
// OpenAI's tutorial on tool calls uses get_weather as an example.
//
// Include it here as a test case based on vendor documentation of
// a model.
//

server.registerTool("get_weather",
  {
    title: "Get Weather",
    description: "Retrieves current weather for the given location",
    inputSchema: { location: z.string() }
  },
  async ({ location }) => ({
    content: [{ type: "text", text: "{\"temperature\": {\"value\": 19.0, \"units\": \"Celsius\"}}" }]
  })
);

//
// Prompts
//

server.registerPrompt(
  "hermes4_system_default",
  {
    title: "Hermes 4 Default System Prompt",
    description: "Default system prompt for Hermes 4",
    argsSchema: { reasoning: z.string() }
  },
  ({ reasoning }) => ({
    messages: [{
      role: "user", // Should be "system"
      content: {
        type: "text",
        text: "You are Hermes, created by Nous Research."
      }
    }]
  })
);

server.registerPrompt(
  "hermes4_system_reasoning",
  {
    title: "Hermes 4 Reasoning System Prompt",
    description: "System prompt to enable chain of thought in Hermes 4.",
    argsSchema: { reasoning: z.string() }
  },
  ({ reasoning }) => ({
    messages: [{
      role: "user", // Should be "system"
      content: {
        type: "text",
        text: "You are a deep thinking AI, you may use extremely long chains of thought to deeply consider the problem and deliberate with yourself via systematic reasoning processes to help come to a correct solution prior to answering. You should enclose your thoughts and internal monologue inside <think> </think> tags, and then provide your solution or response to the problem."
      }
    }]
  })
);

server.registerPrompt(
  "llama_3_1_system_default",
  {
    title: "Llama 3.1 Default System Prompt",
    description: "Default system prompt for Llama 3.1.",
    argsSchema: { reasoning: z.string() }
  },
  ({ reasoning }) => ({
    messages: [{
      role: "user", // Should be "system"
      content: {
        type: "text",
        text: "You are a helpful assistant."
      }
    }]
  })
);

// Start receiving messages on stdin and sending messages on stdout
const transport = new StdioServerTransport();
server.connect(transport);
