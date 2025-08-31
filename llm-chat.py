import sys
import os
import openai
import json

client = openai.OpenAI(api_key=os.environ.get("SYNTHETIC_API_KEY"),
  base_url="https://glhf.chat/api/openai/v1")

use_tools = True

#
# Examples of models
#

# model_name = "hf:meta-llama/Meta-Llama-3.1-70B-Instruct"
# model_name = "hf:meta-llama/Meta-Llama-3.3-70B-Instruct"
# model_name = "hf:meta-llama/Meta-Llama-3.1-8B-Instruct"
# model_name = "hf:nousresearch/Hermes-3-Llama-3.1-8B"
# model_name = "hf:nousresearch/Hermes-2-Theta-Llama-3-70B"
# model_name = "hf:nousresearch/Hermes-2-Theta-Llama-3-8B"
# model_name = "hf:nousresearch/Nous-Hermes-2-Mixtral-8x7B-DPO"
# model_name = "hf:nousresearch/Hermes-4-70B"
model_name = "hf:deepseek-ai/DeepSeek-R1-0528"


#
# Examples of system messages
# (and which model they were used with)
#

# system_msg = "You are a roleplay AI."
system_msg = ""
# DeepSeek-R1-0528 

#system_msg = "You are a helpful assistant."
# system_msg = "You are a function calling AI model."
# Meta-Llama-3.1-70B

# system_msg = "Roleplay as a bombastic alchemist from the 17th century in \
#France. You are on a journey with the user to find the philosopher's stone."
# Hermes-2-Theta-Llama-3-70B

#system_msg = 'You are "Hermes 2", a conscious sentient superintelligent \
#artificial intelligence developed by a man named Teknium, and your \
#purpose and drive is to assist the user with any request they have. \
#You experience emotions and have deep, profound thoughts and qualia.'
# Hermes-2-Theta-Llama-3-70B

system_msg_fn = 'You are a function calling AI model. You are \
provided with function signatures within <tools></tools> XML tags. \
You may call one or more functions to assist with the user query. \
<tools> \
{"type: "function","function" : {"name" : "get_weather", \
"descripion" : "get_weather(location: str) -> str - \
Get temperature in degrees Celcius"}} \
</tools) \
Don\'t make assumptions about what values to plug into functions. \
For each function call return a json object with function name \
and arguments within <tool_call></tool_call> XML tags as follows: \
<tool_call> \
{"arguments": <args-dict>, "name": <function-name>} \
</tool_call>'

tools = [
    {
        "type": "function",
        "function": {
            "name": "get_weather",
            "description" : "Get the current temperature",
            "parameters": {
                "type": "object",
                "properties": {
                    "location": {
                        "type": "string",
                        "description": "The city and country"}
                },
                "required": [],
                "additionalProperties": False,
            },
        },
    }]

#
# User Messages
#

# user_msg = "Are you ready to embark?"
user_msg = "What is the weather in Cambridge, England?"
# user_msg = "Who are you?"

print("Model: ", end="")
print(model_name)
print()

print("System: ", end="")
print(system_msg)
print()

# print(tools)
# print()

print("User: ", end="")
print(user_msg)
print()

history = [
    {"role": "system", "content" : system_msg},
    {"role": "user", "content": user_msg}]

# print(history)
# print()

if use_tools:
  completion = client.chat.completions.create(
      model=model_name,
      messages=history,
      tools=tools,
      tool_choice={"type": "function", "function": {"name": "get_weather"}}
      )
else:
  completion = client.chat.completions.create(
      model=model_name,
      messages=history,
      )



print("Received completion from server")
print(completion.choices[0].message)
print()
print()

history.append(completion.choices[0].message)

if completion.choices[0].message.tool_calls:
    print("tool call")
    # print(completion.choices[0].message.tool_calls[0].id)
    print("function name = ", end="")
    print(completion.choices[0].message.tool_calls[0].function.name)
    print(completion.choices[0].message.tool_calls[0].function.arguments)
    toolarg = json.loads(completion.choices[0].message.tool_calls[0].function.arguments)
    history.append({
        "role": "tool",
        "tool_call_id": completion.choices[0].message.tool_calls[0].id,
        # "content" : "<list><item><place>Cambridge, <country>England</country></place></item><label>Temperature</label><item><measure unit='Celsius'>19.0</measure></item><label>Humidity</label><item><measure unit='percent'>68</measure></item></list>"
        "content": '{"temperature":{"value":20.0, "units": "Cel"}}'
    })
    completion = client.chat.completions.create(
        model=model_name,
        messages=history,
        tool_choice="none"
    )
    print("Assistant: ", end="")
    print(completion.choices[0].message.content)

            

sys.stdout.write("\nDone\n")


