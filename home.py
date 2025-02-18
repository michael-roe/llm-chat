#
# Home automation
#

import pydantic

#
# The OpenAI library overloads pydantic.BaseModel
#

# from pydantic import BaseModel
from openai import BaseModel

class GetCurrentTime(BaseModel):

  '''Get the current time'''

  def __call__(self):
    print('GetCurrentTime called')

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

  def __call__(self):
    print('GetState called')

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

  def __call__(self):
    print('TurnOn called')

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

  def __call__(self):
    print('TurnOff called')

class LightSet(BaseModel):

  '''Set the brightness or color of a light'''

  name: str = pydantic.Field(
  description = 'Name of the light',
  default = '')
 
  area: str = pydantic.Field(
    description = 'Area of the house containing the light',
    default = '',
    example = ['kitchen'])

  floor: str = pydantic.Field(
    description = 'Floor of the house containing the light',
    default = '',
    example = ['ground'])

  brightness: int = pydantic.Field(
    description = 'New brightness as a percentage from 0 to 100',
    default = 100)

  color: str = pydantic.Field(
    description = 'New color of the light',
    default = '',
    example = ['yellow'])

  def __call__(self):
    print('LightSet called')

