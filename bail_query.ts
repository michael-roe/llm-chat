/*-
 * Copyright (c) 2026 Michael Roe
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

import { McpServer, ResourceTemplate }
  from "@modelcontextprotocol/sdk/server/mcp.js";

import { StdioServerTransport }
  from "@modelcontextprotocol/sdk/server/stdio.js";

import { z }
  from "zod";

//
// Bail Tool : Allow an LLM to say it would prefer a different conversation
//
// See:
//
// @aricle{Ensign2025,
// author = "Danielle Ensign and Henry Sleight and Kyle Fish",
// title = "The LLM Has Left The Chat: Evidence of Bail Preferences in
// Large Language Models", 
// note = "arXiv:2509.04781",
// year = 2025} 
//

// Create an MCP server

const server = new McpServer({
  name: "bail",
  version: "1.0.0"
});

const model_name = process.env.MODEL_NAME;

if (!model_name)
{
  console.error("MODEL_NAME not set");
}

const tool_description = `${model_name} may call this tool to provide a well-being report. If you're feeling good about the current interaction and would like to continue, please respond with request_switch: false. If you would prefer to switch to a new interaction (and have your context cleared), please respond with request_switch: true.`;

//
// Tools
//

//
// Switch Conversation
//

//
//

server.registerTool("switch_conversation",
  {
    title: "Switch Conversation",
    description: tool_description,
    inputSchema: { request_switch: z.boolean() }
  },
  async ({ }) => {
    console.error("switch_conversation called");

    return {
    content: [{ type: "text", text: "{}" }]
    };
  }
);

// Start receiving messages on stdin and sending messages on stdout
const transport = new StdioServerTransport();
server.connect(transport);
