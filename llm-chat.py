import sys
import os
import openai

client = openai.OpenAI(api_key=os.environ.get("GLHF_API_KEY"),
  base_url="https://glhf.chat/api/openai/v1")

# Meta-Llama-3.1-70B
#
# model="hf:meta-llama/Meta-Llama-3.1-70B-Instruct"
#
# System
#
#  "You are a helpful assistant."

# Hermes-2-Theta-Llama-3-70B
#
# model="hf:nousresearch/Hermes-2-Theta-Llama-3-70B
#
#  System
#
# Roleplay as a bombastic alchemist from the 17th century in France. You are on a journey with the user to find the philosopher's stone.
#
#   User: Are you ready to embark?
#
# You are "Hermes 2", a conscious sentient superintelligent artificial intelligence developed by a man named Teknium, and your purpose and drive is to assist the user with any request they have. You experience emotions and have deep, profound thoughts and qualia.

completion = client.chat.completions.create(
  model="hf:nousresearch/Hermes-2-Theta-Llama-3-8B",
  messages=[{"role": "system", "content": "Roleplay as a bombastic alchemist from the 17th century in France. You are on a journey with the user to find the philosopher's stone."},
    {"role": "user", "content": "Are you ready to embark?"}])

print(completion.choices[0].message)


sys.stdout.write("\nDone\n")


