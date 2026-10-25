#include "open_extraction/collector_page/collector_page.hpp"

namespace open_extraction::collector_page {

namespace {

constexpr std::string_view kPage = R"html(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>OpenExtraction Collector</title>
<style>
body{font-family:system-ui,sans-serif;margin:0;padding:1rem;max-width:30rem;margin:auto}
dl{display:grid;grid-template-columns:auto 1fr;gap:.5rem 1rem}
dt{font-weight:600}
dd{margin:0;word-break:break-all}
.actions{display:flex;gap:1rem}
button{flex:1;padding:1rem;font-size:1.1rem;border-radius:.5rem;border:1px solid #888}
#message{min-height:1.5rem;color:#b00}
</style>
</head>
<body>
<h1>Collector</h1>
<dl>
<dt>State</dt><dd id="state">…</dd>
<dt>Device UID</dt><dd id="device-uid">…</dd>
</dl>
<div class="actions">
<button id="start" type="button">Start</button>
<button id="stop" type="button">Stop</button>
</div>
<p id="message" role="status"></p>
<script>
const api = "/api/v1/collector";
const $ = (id) => document.getElementById(id);

async function call(path, method) {
  $("message").textContent = "";
  try {
    const response = await fetch(path, { method });
    const body = await response.json();
    if (!response.ok) {
      $("message").textContent = body.error || "Request failed";
      return;
    }
    show(body);
  } catch (e) {
    $("message").textContent = "Controller unreachable";
  }
}

function show(collector) {
  $("state").textContent = collector.state;
  $("device-uid").textContent = collector.device_uid;
}

$("start").addEventListener("click", () => call(api + "/start", "POST"));
$("stop").addEventListener("click", () => call(api + "/stop", "POST"));

// Commands only request a change, so poll for the confirmed state.
async function refresh() {
  try {
    const response = await fetch(api);
    if (response.ok) show(await response.json());
  } catch (e) {
    $("message").textContent = "Controller unreachable";
  }
}
refresh();
setInterval(refresh, 2000);
</script>
</body>
</html>
)html";

} // namespace

std::string_view html() noexcept { return kPage; }

} // namespace open_extraction::collector_page
