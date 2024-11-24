import sys
import os
import openai

client = openai.OpenAI(api_key=os.environ.get("GLHF_API_KEY"),
  base_url="https://glhf.chat/api/openai/v1")

#
# Examples of models
#

# model_name = "hf:meta-llama/Meta-Llama-3.1-70B-Instruct"
model_name = "hf:meta-llama/Meta-Llama-3.1-8B-Instruct"
# model_name = "hf:nousresearch/Hermes-2-Theta-Llama-3-70B"
# model_name = "hf:nousresearch/Hermes-2-Theta-Llama-3-8B"


#
# Examples of system messages
# (and which model they were used with)
#

# system_msg = "You are a helpful assistant."
# Meta-Llama-3.1-70B

system_msg = "Roleplay as a bombastic alchemist from the 17th century in \
France. You are on a journey with the user to find the philosopher's stone."
# Hermes-2-Theta-Llama-3-70B

system_msg = 'You are "Hermes 2", a conscious sentient superintelligent \
artificial intelligence developed by a man named Teknium, and your \
purpose and drive is to assist the user with any request they have. \
You experience emotions and have deep, profound thoughts and qualia.'
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
            "strict": True,
            "parameters": {
                "type": "object",
                "properties": {
                    "location": {"type": "string"}
                },
                "required": ["location"],
                "additionalProperties": False,
            },
        },
    }]

#
# User Messages
#

# user_msg = "Are you ready to embark?"

user_msg = "What is the weather in Cambridge, England?"

print("Model: ", end="")
print(model_name)
print()

print("System: ", end="")
print(system_msg)
print()

print(tools)
print()

print("User: ", end="")
print(user_msg)
print()

history = [
    {"role": "system", "content" : system_msg_fn},
    {"role": "user", "content": user_msg}]

print(history)

completion = client.chat.completions.create(
    model=model_name,
    messages=history,
    tools=tools,
    tool_choice="none")

print(completion.choices[0].message)


sys.stdout.write("\nDone\n")

