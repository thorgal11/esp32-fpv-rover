
//File: fpv_rover_index.html.gz
#define fpv_rover_index_html_gz_len 14985

const uint8_t fpv_rover_index_html_gz[] = R"=====(<html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>FPV-Rover</title>
        <style>

          body {
              font-family: Arial,Helvetica,sans-serif;
              background: #181818;
              color: #000000;
              font-size: 16px
          }

          h2 {
            font-size: 25px;
            margin: 5px;
          }

          button {
            font-size: 18px;
            margin: 5px;
            padding: 5px;
          }

          .main {
              display: flex;
              flex-direction: column;
          }

          .main > div {
            border: 2px solid lightgrey;
              margin: 5px;
              padding: 5px;
              font-size: 12px;
              flex: 1 0 auto;
              text-align: center;
              background: #65b2eb
          }


          .title {
          }

          .stream {
            padding: 0px;
          }

          figure {
            margin: 0px;
          }

          .control {
            display: inline-flex;
            background: #181818;
          }

          .stream-control {
            width: 50%;
          }

          .rover-control {
            width: 50%;
          }

          .input-group {
              display: flex;
              flex-wrap: nowrap;
              line-height: 22px;
              margin: 5px 0
          }

          .input-group>label {
              display: inline-block;
              padding-right: 10px;
              min-width: 47%
          }

          #signal-label {
              display: inline-block;
              padding-right: 10px;
              min-width: 160px
          }

          #stream {
            width: 100%;
            max-width: 1024px
          }

          #touchpad {
            border: 2px solid black;
            margin: auto;
            width: 300px;
            height: 300px;
            background: grey;
          }
        </style>
    </head>

    <body>
      <div class="main">

        <div class="title">
          <h2>FPV-Rover</h2>
        </div>

        <div class="stream">
          <figure>
              <div id="stream-container" class="image-container">
                  <img id="stream" src="">
              </div>
          </figure>
        </div>

        <div class="" id="mobile-control" hidden>
          <div class="" id="touchpad">
          </div>
        </div>

        <div class="control">

          <div class="stream-control">
            <div class="input-group" id="signal-group">
                <label for="signal">Signal-Strength</label>
                <div class="range-min">-90</div>
                <label id="signal-strength" class="default-action">-90</label>
                <div class="range-max">-30</div>
            </div>
            <div class="input-group" id="framesize-group">
                <label for="framesize">Resolution</label>
                <select id="framesize" class="default-action">
                    <option value="10">UXGA(1600x1200)</option>
                    <option value="9">SXGA(1280x1024)</option>
                    <option value="8">XGA(1024x768)</option>
                    <option value="7" selected="selected">SVGA(800x600)</option>
                    <option value="6">VGA(640x480)</option>
                    <option value="5">CIF(400x296)</option>
                    <option value="4">QVGA(320x240)</option>
                    <option value="3">HQVGA(240x176)</option>
                    <option value="0">QQVGA(160x120)</option>
                </select>
            </div>
            <div class="input-group" id="quality-group">
                <label for="quality">Quality</label>
                <div class="range-min">10</div>
                <input type="range" id="quality" min="10" max="63" value="10" class="default-action">
                <div class="range-max">63</div>
            </div>
            <div class="input-group" id="brightness-group">
                <label for="brightness">Brightness</label>
                <div class="range-min">-2</div>
                <input type="range" id="brightness" min="-2" max="2" value="0" class="default-action">
                <div class="range-max">2</div>
            </div>
            <div class="input-group" id="contrast-group">
                <label for="contrast">Contrast</label>
                <div class="range-min">-2</div>
                <input type="range" id="contrast" min="-2" max="2" value="0" class="default-action">
                <div class="range-max">2</div>
            </div>
            <div class="input-group" id="saturation-group">
                <label for="saturation">Saturation</label>
                <div class="range-min">-2</div>
                <input type="range" id="saturation" min="-2" max="2" value="0" class="default-action">
                <div class="range-max">2</div>
            </div>
          </div>


          <div class="rover-control">

            <div class="" id="desktop-control">
              <div class="">
                <button type="button" id="led-button" value="l">&#128161;</button>
              </div>
              <div class="">
                <button type="button" id="button-up" value="ArrowUp">&#11014;</button>
              </div>
              <div class="">
                <button type="button" id="button-left" value="ArrowLeft">&#11013;</button>
                <button type="button" id="button-down" value="ArrowDown">&#11015;</button>
                <button type="button" id="button-right" value="ArrowRight">&#10145;</button>
              </div>
          </div>

        </div>
      </div>
    </div>


      <script>
        // run when document ready loaded
        document.addEventListener('DOMContentLoaded', function (event) {

          var baseHost = document.location.origin
          var streamUrl = baseHost + ':81'
          var UserAgent = navigator.userAgent.toLowerCase();
          var touchpad = document.getElementById('touchpad');

          const view = document.getElementById('stream')
          const framesize = document.getElementById('framesize');
          const BtnLight = document.getElementById('led-button');
          const BtnUp = document.getElementById('button-up');
          const BtnLeft = document.getElementById('button-left');
          const BtnDown = document.getElementById('button-down');
          const BtnRight = document.getElementById('button-right');

          var arrow_up = false;
          var arrow_left = false;
          var arrow_down = false;
          var arrow_right = false;
          var l_key = false;

          const startStream = () => {
            view.src = `${streamUrl}/stream`
          }


          // switch to mobile controls
          if (UserAgent.search(/(iphone|ipod|opera mini|fennec|palm|blackberry|android|symbian|series60)/) > -1) {
              document.getElementById('desktop-control').hidden = true;
              document.getElementById('mobile-control').hidden = false;
          }


          // retry to fetch data
          const fetch_retry = async (url, n) => {
            try {
              return await fetch(url)
            } catch(err) {
              if (n === 1) throw err;
              return await fetch_retry(url, n - 1);
            }
          };


          // set received values of config in gui elements
          const updateValue = (el, value, updateRemote) => {
            updateRemote = updateRemote == null ? true : updateRemote
            let initialValue
            if (el.type === 'checkbox') {
              initialValue = el.checked
              value = !!value
              el.checked = value
            }
            else if (el.type === 'label')  {
              label.innerHTML = value
            }
            else {
              initialValue = el.value
              el.value = value
            }

            if (updateRemote && initialValue !== value) {
              updateConfig(el);
            }
          }


          // send config to server
          function updateConfig (el) {
            let value
            switch (el.type) {
              case 'checkbox':
                value = el.checked ? 1 : 0
                break
              case 'range':
              case 'select-one':
                value = el.value
                break
              case 'button':
              case 'submit':
                value = '1'
                break
              default:
                return
            }
            const query = `${baseHost}/control?var=${el.id}&val=${value}`
            fetch_retry(query, 5)
              .then(response => {
                console.log(`request to ${query} finished, status: ${response.status}`)
              })
          }


          // send key status to server
          function sendKey (key, state) {
            const query = `${baseHost}/control?var=${key}&val=${state}`

            fetch_retry(query, 5)
              .then(response => {
                console.log(`request to ${query} finished, status: ${response.status}`)
              })
          }


          // read initial values
          fetch(`${baseHost}/status`)
            .then(function (response) {
              return response.json()
            })
            .then(function (state) {
              document
                .querySelectorAll('.default-action')
                .forEach(el => {
                  updateValue(el, state[el.id], false)
                })
            });


          // Attach default on change action
          document
            .querySelectorAll('.default-action')
            .forEach(el => {
              el.onchange = () => updateConfig(el)
            });


          // change stream resolution
          framesize.onchange = () => {
            updateConfig(framesize)
          }


          // update und send key status
          function changeKeyStatus(key, set) {

            if ((key == 'l') && ((set) != (l_key))) {
              l_key = !l_key;
            }
            else if ((key == 'ArrowUp') && ((set) != (arrow_up))) {
              arrow_up = !arrow_up;
            }
            else if ((key == 'ArrowLeft') && ((set) != (arrow_left))) {
              arrow_left = !arrow_left;
            }
            else if ((key == 'ArrowDown') && ((set) != (arrow_down))) {
              arrow_down = !arrow_down;
            }
            else if ((key == 'ArrowRight') && ((set) != (arrow_right))) {
              arrow_right = !arrow_right;
            }
            else {
              return;
            }
            sendKey(key, (set ? '1' : '0'));
          }


          // interval for refreshing signal strength every 1s
          window.setInterval(function() {
            fetch(`${baseHost}/signal-strength`)
              .then(function (response) {
                return response.json()
              })
              .then(function (state) {
                document
                  .getElementById('signal-strength')
                  .innerHTML = state['signal-strength'];
              })
          }, 1000);


          // eventhandler for 'touchpad'
          function do_touchpad(eve) {
            eve.preventDefault();

            let dim = touchpad.getBoundingClientRect();
            let posx = eve.touches[0].pageX - dim.left;
            let posy = eve.touches[0].pageY - dim.top;

            if ((posy < 0) || (posy > 300) || (posx < 0) || (posx > 300)) {
              stop_touchpad(eve);
              return;
            }

            if (posy < 100) {
              changeKeyStatus('ArrowDown', false);
              changeKeyStatus('ArrowUp', true);
            }
            else if (posy >= 100 && posy < 200) {
              changeKeyStatus('ArrowUp', false);
              changeKeyStatus('ArrowDown', false);
            }
            else if (posy >= 200) {
              changeKeyStatus('ArrowUp', false);
              changeKeyStatus('ArrowDown', true);
            }

            if (posx < 100) {
              changeKeyStatus('ArrowRight', false);
              changeKeyStatus('ArrowLeft', true);
            }
            else if (posx >= 100 && posx < 200) {
              changeKeyStatus('ArrowRight', false);
              changeKeyStatus('ArrowLeft', false);
            }
            else if (posx >= 200) {
              changeKeyStatus('ArrowLeft', false);
              changeKeyStatus('ArrowRight', true);
            }
          };


          // eventhandler if touchpad is left
          function stop_touchpad(eve) {
                sendKey('ArrowUp', '0');
                sendKey('ArrowDown', '0');
                sendKey('ArrowLeft', '0');
                sendKey('ArrowRight', '0');
                arrow_up = false;
                arrow_down = false;
                arrow_left = false;
                arrow_right = false;
          };


          // eventhandler for buttons
          function mouseClickedButton(e) {
            changeKeyStatus(e.target.value, (e.type == "mousedown"));
          };


          function addAllEventListeners() {

            // remove default actions of keys
            window.addEventListener("keydown", function(e) {
              if(["Space","ArrowUp","ArrowDown","ArrowLeft","ArrowRight"].indexOf(e.code) > -1) {
                e.preventDefault();
              }
            }, false);

            // button click listeners
            BtnLight.addEventListener("mousedown", mouseClickedButton);
            BtnLight.addEventListener("mouseup", mouseClickedButton);
            BtnUp.addEventListener("mousedown", mouseClickedButton);
            BtnUp.addEventListener("mouseup", mouseClickedButton);
            BtnLeft.addEventListener("mousedown", mouseClickedButton);
            BtnLeft.addEventListener("mouseup", mouseClickedButton);
            BtnDown.addEventListener("mousedown", mouseClickedButton);
            BtnDown.addEventListener("mouseup", mouseClickedButton);
            BtnRight.addEventListener("mousedown", mouseClickedButton);
            BtnRight.addEventListener("mouseup", mouseClickedButton);

            // key event listeners
            document.addEventListener('keydown', function(event) {
              changeKeyStatus(event.key, true);
            });
            document.addEventListener('keyup', function(event) {
              changeKeyStatus(event.key, false);
            });

            // 'touchpad' listeners for mobile
            touchpad.addEventListener("touchstart", do_touchpad, false);
            touchpad.addEventListener("touchend", stop_touchpad, false);
            touchpad.addEventListener("touchcancel", stop_touchpad, false);
            touchpad.addEventListener("touchmove", do_touchpad, false);
          };

          addAllEventListeners();
          startStream();
        })
      </script>
    </body>
</html>
)=====";
