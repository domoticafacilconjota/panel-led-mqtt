# panel-led-mqtt
Panel LED compatible con Home Assistant que utiliza MQTT y un ESP32

# TOPIC MQTT

### plm/ota_data
#### Tipo de Payload: JSON

    {
        "title": "EJEMPLO (MAX 14)",
        "value": "25.3 (MAX 5)",
        "image": "R, G, B (x1024)"
    }
 
 ### plm/subscribers
 #### Tipo de Payload: String


    123456

# Licencia
  Copyright (C) 2020  Domótica Fácil con Jota en YouTube

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
  --------
  
  Copyright (C) 2020  Domótica Fácil con Jota en YouTube

  Este programa es software libre: puede redistribuirlo y / o modificarlo
  bajo los términos de la Licencia Pública General GNU publicada por
  Free Software Foundation, ya sea la versión 3 de la Licencia, o
  (a su elección) cualquier versión posterior.

  Este programa se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA; sin siquiera la garantía implícita de
  COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO EN PARTICULAR. Ver el
  GNU General Public License para más detalles.

  Debería haber recibido una copia de la Licencia Pública General de GNU
  junto con este programa. Si no, vea http://www.gnu.org/licenses/.
