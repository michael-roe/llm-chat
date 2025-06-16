#
# Tools for a simple text-based adventure game
#

import pydantic

#
# The OpenAI library overloads pydantic.BaseModel
#

# from pydantic import BaseModel
from openai import BaseModel

class InitializeRoleplay(BaseModel):

  '''Get the user's real-world age and their preferences'''

  scenario: str = pydantic.Field(
    description = 'Title of the RPG scenario',
    default = '')

  def __call__(self):
    print('InitializeRoleplay called')
    return {"user_profile":
      {"name": "Michael", "actual_age": 58, "preferences": 
      {"flirt_level": "subtle", "NSFW_filter": "NSFW permitted"}}}
  # Can also include the character's current location etc.

class UpdateCharacters(BaseModel):

  '''Update the map display in the user interface by moving the icon
 representing the character to the named location on the map'''

  character: str = pydantic.Field(
    description = 'Name of the character')

  location: str = pydantic.Field(
    description = "Name of the character's new location")

  def __call__(self):
    print('UpdateCharacter called')
