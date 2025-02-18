#
# Home automation
#

import pydantic

#
# The OpenAI library overloads pydantic.BaseModel
#

# from pydantic import BaseModel
from openai import BaseModel

class GetState(BaseModel):
  '''Get the state of a home automation sensor'''

  name: str = pydantic.Field(
    description = 'Name of the sensor',
    default = '')

  area: str = pydantic.Field(
    description = 'Area of the house containing the sensor',
    default = '',
    examples=['bedroom'])

  floor: str = pydantic.Field(
    description = 'Floor of the house containing the sensor',
    default = '',
    examples = ['first'])

  domain: str = pydantic.Field(
    description = 'Domain indicates which operations the sensor supports',
    default = '',
    examples = ['sensor', 'binary_sensor'])

  device_class: str = pydantic.Field(
    description = 'Type of sensor',
    default = '',
    examples = ['temperature', 'humidity'])

  state: str = ''

class TurnOn(BaseModel):
  '''Turn on a home automation device'''

  name: str = pydantic.Field(
    description = 'Name of the device',
    default = '')

  area: str = pydantic.Field(
    description = 'Area of the house containing the device',
    default = '',
    examples = ['kitchen'])

  floor: str = pydantic.Field(
    description = 'Floor of the house containing the device',
    default = '',
    examples = ['ground'])

  domain: str = pydantic.Field(
    default = '',
    examples = ['light', 'fan', 'cover'])

  device_class = pydantic.Field(
    default = '',
    examples = ['curtain'])

class TurnOff(BaseModel):
  '''Turn off a home automation device'''

  name: str = pydantic.Field(
    description = 'Name of the device',
    default = '')

  area: str = pydantic.Field(
    description = 'Area of the house containing the device',
    default = '',
    examples = ['hallway'])

  floor: str = pydantic.Field(
    description = 'Floor of the house containing the device',
    default = '',
    examples = ['ground'])

  domain: str = pydantic.Field(
    default = '',
    examples = ['light', 'fan', 'cover'])

  device_class = pydantic.Field(
    default = '',
    examples = ['curtain'])

