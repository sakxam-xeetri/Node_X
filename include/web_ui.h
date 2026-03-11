// ============================================================================
// NodeX — Embedded Web UI (single-header, raw string literals)
//
//   WEB_PORTAL_HTML    — captive portal (portfolio + interactive terminal login)
//   WEB_CONNECTED_HTML — success page after network login
//   WEB_UI_HTML        — admin dashboard (terminal-themed)
// ============================================================================
#pragma once

// ======================== CAPTIVE PORTAL (PORTFOLIO) =========================
const char WEB_PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>sakshyam@arch ~ portfolio</title>
<style>
*,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
::-webkit-scrollbar{width:5px}::-webkit-scrollbar-track{background:#080808}
::-webkit-scrollbar-thumb{background:#2a2a2a;border-radius:3px}
html{font-size:14.5px;scroll-behavior:smooth}
body{background:#080808;color:#e8e8e8;font-family:'Fira Code','Cascadia Code','Consolas',monospace;
line-height:1.65;min-height:100vh;display:flex;flex-direction:column}
.top-bar{position:sticky;top:0;z-index:20;background:#111;border-bottom:1px solid #1e1e1e;
padding:6px 32px;display:flex;align-items:center;gap:16px;font-size:.78rem}
.top-bar-tab{color:#4ec843;background:#1a1a1a;padding:3px 14px;border:1px solid #2a2a2a;
border-bottom:none;border-radius:4px 4px 0 0}
.top-bar-path{color:#555}
.terminal-body{flex:1;padding:24px 32px 32px;width:100%}
.block{margin-bottom:20px;opacity:0;word-wrap:break-word;overflow-wrap:break-word}
.block.visible{opacity:1}
.prompt{color:#4ec843;font-weight:600}
.prompt-user{color:#5b9bd5}.prompt-at{color:#888}.prompt-host{color:#b48ead}
.prompt-path{color:#87ceeb}.prompt-dollar{color:#e06060}
.cmd{color:#fff;font-weight:400}
.ascii-art{color:#fff;font-size:clamp(.55rem,1.32vw,1.05rem);line-height:1.12;
letter-spacing:-.01em;white-space:pre;overflow-x:hidden;display:block;width:100%}
.tagline{color:#e8e8e8;font-weight:700;letter-spacing:4px;font-size:clamp(.75rem,1.1vw,1rem)}
.tagline-dot{color:#e06060}
.role-ai{color:#e06060;font-weight:700}
.role-robot{color:#87ceeb;font-weight:700}
.role-embed{color:#b48ead;font-weight:700}
.white{color:#e0e0e0}.dim{color:#777}
.section-title{font-weight:700;font-size:.95rem;padding-bottom:6px;margin-bottom:10px;
display:block;border-bottom:1px solid #1a1a1a}
.section-title.sky{color:#87ceeb}.section-title.navy{color:#5b9bd5}
.section-title.purple{color:#b48ead}.section-title.yellow{color:#f0c040}
.section-title.red{color:#e06060}
.skill-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(260px,1fr));gap:2px 32px;width:100%}
.skill-item{display:flex;align-items:baseline;gap:6px}
.bullet{color:#e06060;flex-shrink:0}.skill-tag{color:#e0e0e0}
.desc-block{text-align:justify;max-width:100%;color:#e0e0e0;line-height:1.8}
.link{color:#e06060;text-decoration:none}.link:hover{text-decoration:underline;color:#ff8888}
.contact-label{color:#b48ead}
.sep{display:block;width:100%;height:1px;
background:linear-gradient(to right,#1a1a1a,#e06060 40%,#e06060 60%,#1a1a1a);opacity:.18;margin:8px 0}
.cursor-blink{display:inline-block;width:9px;height:17px;background:#4ec843;
animation:blink 1s step-end infinite;vertical-align:text-bottom;margin-left:2px}
@keyframes blink{0%,100%{opacity:1}50%{opacity:0}}
.status-bar{position:sticky;bottom:0;z-index:20;background:#111;border-top:1px solid #1e1e1e;
padding:4px 32px;display:flex;justify-content:space-between;align-items:center;font-size:.72rem;font-weight:600}
.sb-mode{color:#4ec843}.sb-file{color:#888}.sb-info{color:#5b9bd5}.sb-enc{color:#777}.sb-red{color:#e06060}
.terminal-input{background:none;border:none;outline:none;color:#fff;font-family:inherit;
font-size:inherit;width:60%;caret-color:#4ec843}
.terminal-input::placeholder{color:#333}
.boot-ok{color:#4ec843;font-weight:700}.boot-fail{color:#e06060;font-weight:700}
.boot-info{color:#5b9bd5}.boot-dim{color:#555}.boot-purple{color:#b48ead}.boot-white{color:#e8e8e8;font-weight:700}
#boot-screen{position:fixed;inset:0;z-index:999;background:#080808;display:flex;flex-direction:column;
padding:32px;overflow:hidden;font-size:.82rem;line-height:1.5;color:#ccc;transition:opacity .4s}
#boot-screen.fade-out{opacity:0}
#boot-screen .boot-line{white-space:pre}
.skip-hint{position:fixed;bottom:20px;right:20px;color:#333;font-size:.7rem;z-index:1000}
@media(max-width:900px){.terminal-body{padding:18px 20px 28px}
.skill-grid{grid-template-columns:repeat(auto-fill,minmax(200px,1fr))}}
@media(max-width:600px){html{font-size:12px}.terminal-body{padding:12px 12px 20px}
.top-bar,.status-bar{padding:4px 12px}.skill-grid{grid-template-columns:1fr}.tagline{letter-spacing:1px}
.terminal-input{width:50%}}
</style>
</head>
<body>
<div id="boot-screen"></div>
<div class="skip-hint" id="skip-hint">tap to skip</div>
<div class="top-bar" style="display:none" id="top-bar">
  <span class="top-bar-tab">sakshyam@arch:~/portfolio</span>
  <span class="top-bar-path">bash &mdash; 80&times;24</span>
</div>
<div class="terminal-body" id="terminal" style="display:none"></div>
<div class="status-bar" style="display:none" id="status-bar">
  <span><span class="sb-mode">NORMAL</span></span>
  <span class="sb-file">~/portfolio/README.md</span>
  <span><span class="sb-info">ln 1</span> <span class="sb-enc">utf-8</span> <span class="sb-red">linux</span></span>
</div>
<script>
const bootScreen=document.getElementById('boot-screen');
const topBar=document.getElementById('top-bar');
const statusBar=document.getElementById('status-bar');
const terminal=document.getElementById('terminal');

function buildPrompt(){
  return '<span class="prompt"><span class="prompt-user">sakshyam</span>'+
    '<span class="prompt-at">@</span><span class="prompt-host">arch</span>'+
    ':<span class="prompt-path">~</span><span class="prompt-dollar">$</span></span> ';
}

const sections=[
  {type:'command',cmd:'neofetch --ascii',delay:20},
  {type:'ascii',content:
String.raw`███████╗ █████╗ ██╗  ██╗███████╗██╗  ██╗██╗   ██╗ █████╗ ███╗   ███╗
██╔════╝██╔══██╗██║ ██╔╝██╔════╝██║ ██╔╝╚██╗ ██╔╝██╔══██╗████╗ ████║
███████╗███████║█████╔╝ ███████╗█████╔╝  ╚████╔╝ ███████║██╔████╔██║
╚════██║██╔══██║██╔═██╗ ╚════██║██╔═██╗   ╚██╔╝  ██╔══██║██║╚██╔╝██║
███████║██║  ██║██║  ██╗███████║██║  ██╗   ██║   ██║  ██║██║ ╚═╝ ██║
╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝     ╚═╝
██████╗  █████╗ ███████╗████████╗ █████╗ ██╗  ██╗ ██████╗ ████████╗██╗
██╔══██╗██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██║ ██╔╝██╔═══██╗╚══██╔══╝██║
██████╔╝███████║███████╗   ██║   ███████║█████╔╝ ██║   ██║   ██║   ██║
██╔══██╗██╔══██║╚════██║   ██║   ██╔══██║██╔═██╗ ██║   ██║   ██║   ██║
██████╔╝██║  ██║███████║   ██║   ██║  ██║██║  ██╗╚██████╔╝   ██║   ██║
╚═════╝ ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚═╝   ╚═╝`},
  {type:'raw',html:'<span class="tagline">EMBEDDED <span class="tagline-dot">&bull;</span> AI <span class="tagline-dot">&bull;</span> ROBOTICS <span class="tagline-dot">&bull;</span> IOT</span>'},
  {type:'sep'},
  {type:'command',cmd:'cat ./about.md',delay:20},
  {type:'about',roles:[
    {text:'AI & Machine Learning Enthusiast',cls:'role-ai'},
    {text:'Robotics & IoT Developer',cls:'role-robot'},
    {text:'Embedded Systems Engineer',cls:'role-embed'}],
   paragraphs:[
    'I am an AI and Machine Learning enthusiast with a strong passion for robotics, IoT, and embedded systems. My work centers on developing intelligent, real-world solutions through seamless integration of hardware and software.',
    'With hands-on experience across microcontrollers and computing platforms \u2014 including ESP32, ESP8266, Arduino, and Raspberry Pi \u2014 I design and build systems that integrate AI, computer vision, sensor networks, and automation technologies.']},
  {type:'sep'},
  {type:'command',cmd:'ls -la ./skills/',delay:20},
  {type:'skillblock',title:'[ Programming & Software Development ]',titleColor:'sky',
   items:['Python','C / C++','JavaScript','HTML5 / CSS3','React','Tailwind CSS','REST APIs','Git & GitHub','Linux CLI','Arduino IDE','PlatformIO','VS Code']},
  {type:'sep'},
  {type:'command',cmd:'cat ./skills/hardware.list',delay:20},
  {type:'skillblock',title:'[ Hardware & Embedded Systems ]',titleColor:'navy',
   items:['ESP32','ESP8266','Arduino (Uno/Nano/Mega)','Raspberry Pi','NodeMCU','IoT Sensor Integration','Motor Control Systems','L298N Motor Driver','Servo & Stepper Motors','UART / I2C / SPI','PCB Design & Prototyping','Edge Device Development','Embedded C Development']},
  {type:'sep'},
  {type:'command',cmd:'cat ./skills/ai-ml.list',delay:20},
  {type:'skillblock',title:'[ Artificial Intelligence & Machine Learning ]',titleColor:'purple',
   items:['Google Gemini AI','DeepFace','TensorFlow','PyTorch','OpenCV','YOLOv8','Computer Vision Systems','Face Recognition','Emotion Detection','OCR','AI on Edge Devices','Intelligent Automation']},
  {type:'sep'},
  {type:'command',cmd:'cat ./skills/security.list',delay:20},
  {type:'skillblock',title:'[ Cybersecurity & Security Tools ]',titleColor:'yellow',
   items:['Kali Linux','Wireshark','Metasploit Framework','Aircrack-ng','Burp Suite','Shodan','Network Analysis','Wireless Security Testing','Vulnerability Assessment','Penetration Testing','OSINT Techniques']},
  {type:'sep'},
  {type:'command',cmd:'cat ./skills/exploring.list',delay:20},
  {type:'skillblock',title:'[ Currently Learning & Exploring ]',titleColor:'sky',
   items:['Advanced ML Systems','AI-Driven Robotics','Edge AI Deployment','Full-Stack IoT','Secure Embedded Systems','Ethical Hacking & Red Teaming','Cloud-Integrated IoT','DevOps for Intelligent Systems','Scalable System Design']},
  {type:'sep'},
  {type:'command',cmd:'cat ./approach.md',delay:20},
  {type:'textblock',title:'[ Professional Approach ]',titleColor:'yellow',
   text:'Focus on designing scalable, production-oriented systems rather than limited prototypes. Actively exploring emerging technologies in AI-driven robotics, intelligent automation, and cybersecurity, with a strong commitment to continuous learning and open-source collaboration.'},
  {type:'sep'},
  {type:'command',cmd:'cat ./objective.md',delay:20},
  {type:'textblock',title:'[ Career Objective ]',titleColor:'navy',
   text:'To grow as a full-stack hardware\u2013software developer and contribute to impactful innovations in artificial intelligence, robotics, and smart technologies that solve meaningful real-world problems.'},
  {type:'sep'},
  {type:'command',cmd:'cat ./contact.json',delay:20},
  {type:'contact',title:'[ Contact Information ]'},
  {type:'sep'},
  {type:'command',cmd:'cat ./socials.json',delay:20},
  {type:'socials',title:'[ Social & Professional Profiles ]'},
  {type:'sep'},
  {type:'motd'},
  {type:'interactive'}
];

function el(tag,cls){const e=document.createElement(tag);if(cls)e.className=cls;return e;}
function createBlock(){const d=el('div','block');terminal.appendChild(d);return d;}
function sleep(ms){return new Promise(r=>setTimeout(r,ms));}
function scrollEnd(){window.scrollTo({top:document.body.scrollHeight,behavior:'smooth'});}
function escHtml(s){return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}

async function typeInto(node,text,delay){
  for(let i=0;i<text.length;i++){node.textContent+=text[i];if(i%4===0)scrollEnd();if(delay>0)await sleep(delay);}
}

async function renderCommand(s){const b=createBlock();b.classList.add('visible');b.innerHTML=buildPrompt();
  const c=el('span','cmd');b.appendChild(c);await typeInto(c,s.cmd,s.delay);await sleep(80);}
async function renderAscii(s){const b=createBlock();b.classList.add('visible');
  const p=el('div','ascii-art');p.textContent=s.content;b.appendChild(p);scrollEnd();await sleep(60);}
async function renderRaw(s){const b=createBlock();b.classList.add('visible');b.innerHTML=s.html;scrollEnd();await sleep(60);}
async function renderSep(){const b=createBlock();b.classList.add('visible');b.appendChild(el('div','sep'));scrollEnd();await sleep(20);}

async function renderAbout(s){const b=createBlock();b.classList.add('visible');
  for(const r of s.roles){const d=el('div',r.cls);d.textContent=r.text;b.appendChild(d);}
  b.appendChild(document.createElement('br'));
  for(const p of s.paragraphs){const d=el('div','desc-block');d.textContent=p;b.appendChild(d);b.appendChild(document.createElement('br'));}
  scrollEnd();await sleep(60);}

async function renderSkillBlock(s){const b=createBlock();b.classList.add('visible');
  const t=el('div','section-title '+(s.titleColor||'sky'));t.textContent=s.title;b.appendChild(t);
  const g=el('div','skill-grid');b.appendChild(g);
  for(const item of s.items){const row=el('div','skill-item');
    const bul=el('span','bullet');bul.textContent='\u25b8';
    const txt=el('span','skill-tag');txt.textContent=item;
    row.appendChild(bul);row.appendChild(txt);g.appendChild(row);}
  scrollEnd();await sleep(60);}

async function renderTextBlock(s){const b=createBlock();b.classList.add('visible');
  const t=el('div','section-title '+(s.titleColor||'sky'));t.textContent=s.title;b.appendChild(t);
  const d=el('div','desc-block');d.textContent=s.text;b.appendChild(d);scrollEnd();await sleep(60);}

async function renderContact(s){const b=createBlock();b.classList.add('visible');
  const t=el('div','section-title red');t.textContent=s.title;b.appendChild(t);
  const entries=[
    {lbl:'Location',val:'Kathmandu, Nepal',link:null},
    {lbl:'Email   ',val:'sakshyamxeetri@gmail.com',link:'mailto:sakshyamxeetri@gmail.com'},
    {lbl:'Phone   ',val:'+977 97643 20750',link:'tel:+9779764320750'},
    {lbl:'Website ',val:'sakshyambastakoti.com.np',link:'https://sakshyambastakoti.com.np'}];
  for(const e of entries){const row=el('div','skill-item');
    const bul=el('span','bullet');bul.textContent='\u25b8';
    const lbl=el('span','contact-label');lbl.textContent=e.lbl+' : ';
    row.appendChild(bul);row.appendChild(lbl);
    if(e.link){const a=document.createElement('a');a.className='link';a.href=e.link;
      a.target='_blank';a.rel='noopener noreferrer';a.textContent=e.val;row.appendChild(a);}
    else{const v=el('span','white');v.textContent=e.val;row.appendChild(v);}
    b.appendChild(row);}scrollEnd();await sleep(60);}

async function renderSocials(s){const b=createBlock();b.classList.add('visible');
  const t=el('div','section-title red');t.textContent=s.title;b.appendChild(t);
  const socials=[
    {lbl:'LinkedIn ',url:'https://www.linkedin.com/in/sakshyambastakoti/',txt:'linkedin.com/in/sakshyambastakoti'},
    {lbl:'GitHub   ',url:'https://github.com/sakxam-xeetri',txt:'github.com/sakxam-xeetri'},
    {lbl:'Instagram',url:'https://www.instagram.com/sakxam_console.log/',txt:'instagram.com/sakxam_console.log'}];
  for(const e of socials){const row=el('div','skill-item');
    const bul=el('span','bullet');bul.textContent='\u25b8';
    const lbl=el('span','contact-label');lbl.textContent=e.lbl+' : ';
    row.appendChild(bul);row.appendChild(lbl);
    const a=document.createElement('a');a.className='link';a.href=e.url;
    a.target='_blank';a.rel='noopener noreferrer';a.textContent=e.txt;row.appendChild(a);
    b.appendChild(row);}scrollEnd();await sleep(60);}

async function renderMotd(){
  const b=createBlock();b.classList.add('visible');
  b.innerHTML='<span class="dim">\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501</span><br>'+
    '<span class="white">  NodeX Wi-Fi Repeater \u2014 Captive Portal</span><br>'+
    '<span class="dim">  Type </span><span class="boot-ok">help</span><span class="dim"> for available commands</span><br>'+
    '<span class="dim">\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501</span>';
  scrollEnd();await sleep(80);}

const cmdHistory=[];let historyIdx=-1;let pendingInput='';

function newPrompt(){
  const b=createBlock();b.classList.add('visible');b.innerHTML=buildPrompt();
  const inp=document.createElement('input');inp.type='text';inp.className='terminal-input';
  inp.spellcheck=false;inp.autocomplete='off';inp.placeholder='';
  b.appendChild(inp);inp.focus();scrollEnd();
  inp.addEventListener('keydown',handleInput);
}
function handleInput(e){
  const inp=e.target;
  if(e.key==='ArrowUp'){
    e.preventDefault();
    if(historyIdx===-1)pendingInput=inp.value;
    if(historyIdx<cmdHistory.length-1){historyIdx++;inp.value=cmdHistory[cmdHistory.length-1-historyIdx];}
    inp.setSelectionRange(inp.value.length,inp.value.length);return;
  }
  if(e.key==='ArrowDown'){
    e.preventDefault();
    if(historyIdx>0){historyIdx--;inp.value=cmdHistory[cmdHistory.length-1-historyIdx];}
    else if(historyIdx===0){historyIdx=-1;inp.value=pendingInput;}
    inp.setSelectionRange(inp.value.length,inp.value.length);return;
  }
  if(e.key!=='Enter')return;
  const raw=inp.value;
  const cmd=raw.trim().toLowerCase().replace(/\s+/g,' ');
  if(raw.trim()){cmdHistory.push(raw.trim());historyIdx=-1;pendingInput='';}
  const span=el('span','cmd');span.textContent=raw;
  inp.parentElement.replaceChild(span,inp);
  processCmd(cmd,raw);
}
async function processCmd(cmd,raw){
  if(cmd==='help'||cmd==='--help'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title sky">[ Available Commands ]</span>'+
      '<div class="skill-grid" style="gap:4px 32px">'+
      '<div><span class="bullet">\u25b8</span> <span class="white">login: &lt;code&gt;</span> <span class="dim">\u2014 authenticate to network or admin</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">help</span>         <span class="dim">\u2014 show this list</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">clear</span>        <span class="dim">\u2014 clear terminal</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">whoami</span>       <span class="dim">\u2014 current user</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">about</span>        <span class="dim">\u2014 about sakshyam</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">skills</span>       <span class="dim">\u2014 list skills</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">contact</span>      <span class="dim">\u2014 contact info</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">wifi</span>         <span class="dim">\u2014 network info</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">status</span>       <span class="dim">\u2014 portal status</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">ls</span>           <span class="dim">\u2014 list directory</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">pwd</span>          <span class="dim">\u2014 working directory</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">uname</span>        <span class="dim">\u2014 system info</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="white">date</span>         <span class="dim">\u2014 current date &amp; time</span></div>'+
      '</div>'+
      '<div style="margin-top:6px;color:#555;font-size:.8rem">\u2191 / \u2193 arrow keys browse command history</div>';
    scrollEnd();newPrompt();
  } else if(cmd==='clear'){
    terminal.innerHTML='';newPrompt();
  } else if(cmd==='whoami'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML='<span class="prompt-user">sakshyam</span><span class="prompt-at"> @ </span><span class="prompt-host">arch</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='pwd'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML='<span class="white">/home/sakshyam/portfolio</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='uname'||cmd==='uname -a'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML='<span class="dim">Linux arch 6.8.2-arch1-1 #1 SMP PREEMPT_DYNAMIC x86_64 GNU/Linux</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='date'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML='<span class="dim">'+new Date().toUTCString()+'</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='ls'||cmd==='ls -la'||cmd==='ls -l'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="dim">total 64</span><br>'+
      '<span class="dim">drwxr-xr-x sakshyam sakshyam 4096</span> <span class="prompt-path">.</span><br>'+
      '<span class="dim">-rw-r--r-- sakshyam sakshyam 3.2K</span> <span class="white">about.md</span><br>'+
      '<span class="dim">-rw-r--r-- sakshyam sakshyam 1.1K</span> <span class="white">contact.json</span><br>'+
      '<span class="dim">-rw-r--r-- sakshyam sakshyam  892</span> <span class="white">objective.md</span><br>'+
      '<span class="dim">-rw-r--r-- sakshyam sakshyam 1.4K</span> <span class="white">approach.md</span><br>'+
      '<span class="dim">-rw-r--r-- sakshyam sakshyam  744</span> <span class="white">socials.json</span><br>'+
      '<span class="dim">drwxr-xr-x sakshyam sakshyam 4096</span> <span class="prompt-path">skills/</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='about'||cmd==='cat about.md'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title purple">[ About ]</span>'+
      '<span class="role-ai">AI &amp; Machine Learning Enthusiast</span><br>'+
      '<span class="role-robot">Robotics &amp; IoT Developer</span><br>'+
      '<span class="role-embed">Embedded Systems Engineer</span><br><br>'+
      '<span class="desc-block">AI and Machine Learning enthusiast with expertise in robotics, IoT, and embedded systems. Passionate about building intelligent real-world solutions through seamless hardware\u2013software integration.</span>';
    scrollEnd();newPrompt();
  } else if(cmd==='skills'||cmd==='cat skills.list'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title sky">[ Core Skills ]</span>'+
      '<div class="skill-grid">'+
      ['Python','C / C++','JavaScript','ESP32','Arduino','Raspberry Pi','TensorFlow','OpenCV','YOLOv8','Linux','Kali Linux','PlatformIO'].map(s=>
        '<div><span class="bullet">\u25b8</span> <span class="skill-tag">'+escHtml(s)+'</span></div>').join('')+
      '</div>';
    scrollEnd();newPrompt();
  } else if(cmd==='contact'||cmd==='cat contact.json'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title red">[ Contact ]</span>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Email   :</span> <a class="link" href="mailto:sakshyamxeetri@gmail.com">sakshyamxeetri@gmail.com</a></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Phone   :</span> <a class="link" href="tel:+9779764320750">+977 97643 20750</a></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Website :</span> <a class="link" href="https://sakshyambastakoti.com.np" target="_blank" rel="noopener noreferrer">sakshyambastakoti.com.np</a></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">GitHub  :</span> <a class="link" href="https://github.com/sakxam-xeetri" target="_blank" rel="noopener noreferrer">github.com/sakxam-xeetri</a></div>';
    scrollEnd();newPrompt();
  } else if(cmd==='wifi'||cmd==='iwconfig'||cmd==='ip a'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title navy">[ Network ]</span>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">SSID    :</span> <span class="white">NodeX</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Type    :</span> <span class="white">Open Wi-Fi (Captive Portal)</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Gateway :</span> <span class="white">192.168.4.1</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Auth    :</span> <span class="dim">Type <span class="boot-ok">login: &lt;code&gt;</span> to unlock internet</span></div>';
    scrollEnd();newPrompt();
  } else if(cmd==='status'){
    const b=createBlock();b.classList.add('visible');
    b.innerHTML=
      '<span class="section-title yellow">[ Portal Status ]</span>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Portal  :</span> <span class="boot-ok">Active</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">DNS     :</span> <span class="boot-ok">Running</span></div>'+
      '<div><span class="bullet">\u25b8</span> <span class="contact-label">Auth    :</span> <span class="dim">Not authenticated</span></div>';
    scrollEnd();newPrompt();
  } else if(cmd.startsWith('login')){
    const code=cmd.slice(5).replace(/^[\s:]+/,'').trim();
    if(!code){const b=createBlock();b.classList.add('visible');
      b.innerHTML='<span class="boot-fail">Usage: login: &lt;code&gt;</span>';scrollEnd();newPrompt();return;}
    await doLogin(code);
  } else if(cmd===''){newPrompt();}
  else{const b=createBlock();b.classList.add('visible');
    b.innerHTML='<span class="boot-fail">bash: '+escHtml(raw.split(' ')[0])+': command not found</span>';
    scrollEnd();newPrompt();}
}
function appendLine(html){const b=createBlock();b.classList.add('visible');b.innerHTML=html;scrollEnd();}

async function doLogin(code){
  const b=createBlock();b.classList.add('visible');
  b.innerHTML='<span class="dim">Authenticating\u2026</span>';scrollEnd();
  try{
    const r=await fetch('/portal/login',{method:'POST',
      headers:{'Content-Type':'application/x-www-form-urlencoded'},
      body:'code='+encodeURIComponent(code)});
    const d=await r.json();
    if(d.status==='admin'){
      b.innerHTML='<span class="boot-ok">[\u2713] Admin authentication successful</span>';scrollEnd();
      await sleep(400);appendLine('<span class="dim">Loading admin dashboard\u2026</span>');
      await sleep(700);window.location.href='/admin';
    } else if(d.status==='connected'){
      b.innerHTML='<span class="boot-ok">[\u2713] Network access granted</span>';scrollEnd();
      await sleep(300);appendLine('<span class="boot-ok">[\u2713] DNS forwarding active</span>');
      await sleep(300);appendLine('<span class="boot-ok">[\u2713] Internet access enabled</span>');
      await sleep(300);
      appendLine(
        '<span class="dim">\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501</span><br>'+
        '<span class="white">  \u2713 Connected \u2014 internet access is now active.</span><br>'+
        '<span class="dim">  You may close this page and browse normally from any app.</span><br>'+
        '<span class="dim">\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501</span>'
      );
      // Navigate the CNA browser to the OS captive-portal success URL.
      // iOS needs <body>Success</body>; Android needs HTTP 204.
      // /hotspot-detect.html returns the iOS success HTML for authenticated
      // clients, and most Android CNAs also accept any non-redirect 2xx.
      // This closes the mini-browser gracefully so Wi-Fi stays connected.
      await sleep(1800);
      window.location.replace('http://'+location.hostname+'/hotspot-detect.html');
    } else {throw new Error('denied');}
  }catch(e){
    b.innerHTML='<span class="boot-fail">[\u2717] Access denied \u2014 invalid login code</span>';
    scrollEnd();newPrompt();
  }
}

async function run(){
  for(const s of sections){
    switch(s.type){
      case 'command':await renderCommand(s);break;
      case 'ascii':await renderAscii(s);break;
      case 'raw':await renderRaw(s);break;
      case 'sep':await renderSep();break;
      case 'about':await renderAbout(s);break;
      case 'skillblock':await renderSkillBlock(s);break;
      case 'textblock':await renderTextBlock(s);break;
      case 'contact':await renderContact(s);break;
      case 'socials':await renderSocials(s);break;
      case 'motd':await renderMotd();break;
      case 'interactive':newPrompt();break;
    }
  }
}

// Click anywhere to focus the active input
document.addEventListener('click',()=>{const i=document.querySelector('.terminal-input');if(i)i.focus();});

/* ── BOOT SEQUENCE ── */
const bootLines=[
  {text:'BIOS v3.14.1 \u2014 Sakshyam Systems Inc.',cls:'boot-white',d:60},
  {text:'Copyright (C) 2024-2026. All rights reserved.',cls:'boot-dim',d:40},
  {text:'',d:80},
  {text:'CPU: Intel(R) Core(TM) i7-13700K @ 5.40GHz',cls:'',d:30},
  {text:'RAM: 32768 MB DDR5 \u2014 OK',cls:'',d:30,tag:'ok'},
  {text:'GPU: NVIDIA RTX 4070 Ti \u2014 Detected',cls:'',d:30,tag:'ok'},
  {text:'NVMe SSD: 1TB \u2014 /dev/nvme0n1',cls:'',d:30},
  {text:'',d:60},
  {text:'Initializing kernel 6.8.2-arch1-1 ...',cls:'boot-info',d:70},
  {text:'[    0.000000] Linux version 6.8.2-arch1-1 (linux@archlinux)',cls:'boot-dim',d:20},
  {text:'[    0.004521] Command line: BOOT_IMAGE=/vmlinuz-linux',cls:'boot-dim',d:15},
  {text:'[    0.012744] x86/cpu: SGX disabled by BIOS',cls:'boot-dim',d:15},
  {text:'[    0.048103] ACPI: RSDP 0x00000000000F0490',cls:'boot-dim',d:15},
  {text:'[    0.091002] PCI: Using configuration type 1',cls:'boot-dim',d:15},
  {text:'',d:40},
  {text:':: running early hook [udev]',cls:'',d:35},
  {text:'Starting systemd-udevd... ',cls:'',d:35,tag:'ok'},
  {text:':: running hook [encrypt]',cls:'',d:25},
  {text:':: running hook [filesystems]',cls:'',d:25},
  {text:'Mounting /dev/nvme0n1p2 on / ...',cls:'',d:40,tag:'ok'},
  {text:'Mounting /dev/nvme0n1p1 on /boot ...',cls:'',d:30,tag:'ok'},
  {text:'',d:40},
  {text:'systemd 255.4-2-arch started.',cls:'boot-info',d:40},
  {text:'[ OK ] Started Network Manager.',cls:'',d:25,tag:'ok'},
  {text:'[ OK ] Reached target Network.',cls:'',d:20,tag:'ok'},
  {text:'[ OK ] Started OpenSSH Daemon.',cls:'',d:20,tag:'ok'},
  {text:'[ OK ] Started Firewall (nftables).',cls:'',d:20,tag:'ok'},
  {text:'[ OK ] Reached target Multi-User System.',cls:'',d:30,tag:'ok'},
  {text:'',d:35},
  {text:'Loading portfolio environment...',cls:'boot-purple',d:50},
  {text:'  > node_modules        \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > skill_registry      \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > ai_models           \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > hardware_drivers    \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > security_tools      \u2713',cls:'',d:25,tag:'ok'},
  {text:'',d:30},
  {text:'Loading NodeX Wi-Fi Repeater...',cls:'boot-purple',d:50},
  {text:'  > captive_portal      \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > dns_server          \u2713',cls:'',d:25,tag:'ok'},
  {text:'  > napt_forwarding     \u2713',cls:'',d:25,tag:'ok'},
  {text:'',d:30},
  {text:'All systems operational.',cls:'boot-white',d:60},
  {text:'',d:30},
  {text:'arch login: sakshyam',cls:'',d:70},
  {text:'Password: \u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022\u2022',cls:'boot-dim',d:80},
  {text:'',d:40},
  {text:'Welcome to Arch Linux \u2014 sakshyam@arch',cls:'boot-info',d:60},
  {text:'__LASTLOGIN__',cls:'boot-dim',d:40},
  {text:'',d:50},
  {text:'Launching terminal session...',cls:'boot-purple',d:150}
];

(function(){
  const key='sb_last_login';const prev=localStorage.getItem(key);
  const now=new Date().toString().replace(/\s*\(.*\)$/,'');
  const display=prev?'Last login: '+prev:'Last login: first session';
  localStorage.setItem(key,now);
  const idx=bootLines.findIndex(l=>l.text==='__LASTLOGIN__');
  if(idx!==-1)bootLines[idx].text=display;
})();

let skipBoot=false;
bootScreen.addEventListener('click',()=>{skipBoot=true;});
bootScreen.addEventListener('touchstart',()=>{skipBoot=true;});

function bootLine(html){
  const d=document.createElement('div');d.className='boot-line';d.innerHTML=html;
  bootScreen.appendChild(d);bootScreen.scrollTop=bootScreen.scrollHeight;
}

async function runBoot(){
  for(const l of bootLines){
    if(skipBoot)break;
    if(l.text===''){bootLine('&nbsp;');}
    else if(l.tag==='ok'){
      const parts=l.text.split('\u2713');
      if(parts.length===2){bootLine('<span class="'+l.cls+'">'+parts[0]+'</span><span class="boot-ok">\u2713</span>');}
      else if(l.text.startsWith('[ OK ]')){bootLine('<span class="boot-ok">[ OK ]</span><span class="'+(l.cls||'')+'">'+l.text.slice(6)+'</span>');}
      else{const idx=l.text.lastIndexOf('\u2014');
        if(idx>-1){bootLine('<span class="'+l.cls+'">'+l.text.slice(0,idx+1)+'</span> <span class="boot-ok">'+l.text.slice(idx+2)+'</span>');}
        else{bootLine('<span class="'+l.cls+'">'+l.text+'</span> <span class="boot-ok">OK</span>');}}
    }else{bootLine('<span class="'+l.cls+'">'+l.text+'</span>');}
    await sleep(l.d);
  }
  await sleep(200);
  document.getElementById('skip-hint').style.display='none';
  bootScreen.classList.add('fade-out');await sleep(400);
  bootScreen.style.display='none';
  topBar.style.display='';terminal.style.display='';statusBar.style.display='';
  run();
}
runBoot();
</script>
</body>
</html>
)rawliteral";

// ======================== "CONNECTED" SUCCESS PAGE ==========================
const char WEB_CONNECTED_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Connected — NodeX</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#080808;color:#e8e8e8;font-family:'Fira Code','Cascadia Code','Consolas',monospace;
line-height:1.65;min-height:100vh;padding:32px}
.top-bar{background:#111;border-bottom:1px solid #1e1e1e;padding:6px 32px;margin:-32px -32px 24px;
font-size:.78rem;display:flex;align-items:center;gap:16px}
.top-bar-tab{color:#4ec843;background:#1a1a1a;padding:3px 14px;border:1px solid #2a2a2a;
border-bottom:none;border-radius:4px 4px 0 0}
.top-bar-path{color:#555}
.prompt{color:#4ec843;font-weight:600}
.ok{color:#4ec843;font-weight:700}
.dim{color:#777}.white{color:#e0e0e0}
.line{margin-bottom:6px}
.cursor-blink{display:inline-block;width:9px;height:17px;background:#4ec843;
animation:blink 1s step-end infinite;vertical-align:text-bottom;margin-left:2px}
@keyframes blink{0%,100%{opacity:1}50%{opacity:0}}
</style>
</head>
<body>
<div class="top-bar">
  <span class="top-bar-tab">sakshyam@arch:~</span>
  <span class="top-bar-path">connected</span>
</div>
<div class="line"><span class="prompt">sakshyam@arch:~$</span> <span class="white">login: dev</span></div>
<div class="line">&nbsp;</div>
<div class="line"><span class="ok">[&#10003;] Network access granted</span></div>
<div class="line"><span class="ok">[&#10003;] DNS forwarding active</span></div>
<div class="line"><span class="ok">[&#10003;] Internet access enabled</span></div>
<div class="line">&nbsp;</div>
<div class="line"><span class="white">You are now connected to the internet.</span></div>
<div class="line"><span class="dim">You can close this page and browse normally.</span></div>
<div class="line">&nbsp;</div>
<div class="line"><span class="prompt">sakshyam@arch:~$</span> <span class="cursor-blink">&nbsp;</span></div>
</body>
</html>
)rawliteral";

// ====================== ADMIN DASHBOARD (terminal-themed) ====================
const char WEB_UI_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>NodeX Admin Dashboard</title>
<style>
*,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
::-webkit-scrollbar{width:5px}::-webkit-scrollbar-track{background:#080808}
::-webkit-scrollbar-thumb{background:#2a2a2a;border-radius:3px}
html{font-size:14px}
body{background:#080808;color:#e8e8e8;font-family:'Fira Code','Cascadia Code','Consolas',monospace;
line-height:1.65;min-height:100vh;display:flex;flex-direction:column}
.top-bar{position:sticky;top:0;z-index:20;background:#111;border-bottom:1px solid #1e1e1e;
padding:6px 32px;display:flex;align-items:center;gap:16px;font-size:.78rem}
.top-bar-tab{color:#4ec843;background:none;padding:3px 14px;border:none}
.top-bar-path{color:#555}
.main{flex:1;padding:24px 32px 32px;width:100%;max-width:1000px;margin:0 auto}
.status-bar{position:sticky;bottom:0;z-index:20;background:#111;border-top:1px solid #1e1e1e;
padding:4px 32px;display:flex;justify-content:space-between;align-items:center;font-size:.72rem;font-weight:600}
.sb-mode{color:#4ec843}.sb-file{color:#888}.sb-info{color:#5b9bd5}.sb-red{color:#e06060}
.prompt{color:#4ec843;font-weight:600}
.cmd{color:#fff}.dim{color:#777}.white{color:#e0e0e0}
.ok{color:#4ec843;font-weight:700}.fail{color:#e06060;font-weight:700}
.section-title{font-weight:700;font-size:.9rem;padding:0;margin:14px 0 4px;
display:block;border:none}
.section-title::after{content:'';display:block;border-bottom:1px dashed #333;margin-top:4px}
.section-title.sky{color:#87ceeb}.section-title.navy{color:#5b9bd5}
.section-title.purple{color:#b48ead}.section-title.red{color:#e06060}
.section-title.yellow{color:#f0c040}
.stat{display:flex;gap:8px;padding:3px 0;font-size:.85rem}
.stat .lbl{color:#b48ead;min-width:140px;flex-shrink:0}
.stat .val{color:#e0e0e0}
.stat .val.up{color:#4ec843}.stat .val.down{color:#e06060}
.bullet{color:#e06060}
.rssi-bar{display:inline-block;width:60px;height:8px;background:#1e1e1e;border-radius:0;
overflow:hidden;vertical-align:middle;margin-left:6px}
.rssi-bar .fill{height:100%;border-radius:0;transition:width .4s}
.tabs{display:flex;gap:0;margin-bottom:20px;flex-wrap:wrap;border-bottom:1px dashed #333;padding-bottom:6px}
.tab{padding:4px 6px;margin:0 4px;background:none;border:none;border-bottom:2px solid transparent;
cursor:pointer;font-size:.8rem;color:#555;transition:.2s;font-family:inherit}
.tab::before{content:'[ ';color:#333}.tab::after{content:' ]';color:#333}
.tab:hover{color:#e06060}.tab:hover::before,.tab:hover::after{color:#555}
.tab.active{color:#e06060;border-bottom-color:#e06060}.tab.active::before,.tab.active::after{color:#e06060}
.tab.logout{margin-left:auto;color:#555}.tab.logout::before,.tab.logout::after{content:''}
.tab.logout:hover{color:#e06060}
.panel{display:none}.panel.active{display:block}
table{width:100%;border-collapse:collapse;font-size:.82rem;margin-top:8px}
th{text-align:left;padding:6px 8px;color:#87ceeb;border-bottom:1px dashed #333;
font-weight:600;text-transform:uppercase;letter-spacing:.5px;font-size:.72rem}
td{padding:6px 8px;border-bottom:1px dashed #1a1a1a;color:#e0e0e0}
tr:hover td{background:rgba(255,255,255,.02)}
.form-group{margin-bottom:12px}
.form-group label{display:block;font-size:.75rem;color:#b48ead;margin-bottom:3px;
text-transform:uppercase;letter-spacing:.5px}
.form-group input,.form-group select{width:100%;padding:6px 10px;background:#0a0a0a;
border:1px dashed #333;border-radius:0;color:#e0e0e0;font-size:.85rem;font-family:inherit}
.form-group input:focus{outline:none;border-color:#e06060;border-style:solid;box-shadow:none}
.form-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(280px,1fr));gap:4px 24px}
.card{background:none;border:none;border-radius:0;padding:14px 0;border-top:1px dashed #222}
.card:first-child{border-top:none;padding-top:0}
.card h3{font-size:.8rem;color:#e06060;text-transform:uppercase;letter-spacing:1px;margin-bottom:10px}
.card h3::before{content:'## ';color:#555}
.btn{padding:6px 16px;border:1px dashed #444;border-radius:0;font-size:.82rem;font-weight:600;
cursor:pointer;transition:.2s;font-family:inherit;background:none;color:#e0e0e0}
.btn:hover{border-color:#e06060;color:#e06060;border-style:solid}
.btn-red{border-color:#e06060;color:#e06060}.btn-red:hover{background:#e06060;color:#fff}
.btn-green{border-color:#4ec843;color:#4ec843}.btn-green:hover{background:#4ec843;color:#000}
.btn-blue{border-color:#5b9bd5;color:#5b9bd5}.btn-blue:hover{background:#5b9bd5;color:#fff}
.btn-sm{padding:5px 12px;font-size:.78rem}
.btns{display:flex;gap:8px;margin-top:12px;flex-wrap:wrap}
.toast{position:fixed;bottom:24px;right:24px;padding:10px 18px;border-radius:0;color:#fff;
font-size:.82rem;font-weight:600;z-index:999;opacity:0;transition:.3s;transform:translateY(20px);border:1px dashed #444}
.toast.show{opacity:1;transform:translateY(0)}.toast.ok{background:#4ec843}.toast.err{background:#e06060}
.hidden{display:none}
@media(max-width:600px){.main{padding:12px}.top-bar,.status-bar{padding:4px 12px}
.form-grid{grid-template-columns:1fr}.stat .lbl{min-width:100px}}
</style>
</head>
<body>
<div class="top-bar">
  <span class="top-bar-tab">admin@nodex:~/dashboard</span>
  <span class="top-bar-path">bash &mdash; 80&times;24</span>
</div>
<div class="main">
  <div style="margin-bottom:18px">
    <span class="prompt">admin@nodex:~$</span> <span class="cmd">welcome</span>
    <div style="margin-top:6px;font-size:.85rem">
      <span class="white" style="font-weight:700">Node</span><span class="fail" style="font-weight:700">X</span>
      <span class="dim">Admin Dashboard v1.0.0</span>
      <span class="dim" style="margin-left:12px" id="uptimeLabel"></span>
    </div>
  </div>
  <div class="tabs">
    <div class="tab active" data-tab="status">status</div>
    <div class="tab" data-tab="clients">clients</div>
    <div class="tab" data-tab="networks">networks</div>
    <div class="tab" data-tab="config">config</div>
    <div class="tab" data-tab="tools">tools</div>
    <div class="tab logout" onclick="logout()">logout</div>
  </div>

  <!-- STATUS -->
  <div class="panel active" id="p-status">
    <div><span class="prompt">admin@nodex:~$</span> <span class="cmd">show status</span></div>
    <div class="section-title sky">###### System</div>
    <div id="sysInfo"></div>
    <div class="section-title navy">###### Upstream &mdash; STA</div>
    <div id="staInfo"></div>
    <div class="section-title purple">###### Repeater &mdash; AP</div>
    <div id="apInfo"></div>
  </div>

  <!-- CLIENTS -->
  <div class="panel" id="p-clients">
    <div><span class="prompt">admin@nodex:~$</span> <span class="cmd">show clients</span></div>
    <div class="section-title sky">###### Connected Devices</div>
    <div class="dim" style="font-size:.78rem;margin-bottom:6px">Auth = portal session active &bull; Deauth = revoke session (re-login required) &bull; Kick = disconnect from Wi-Fi</div>
    <table><thead><tr><th>#</th><th>MAC Address</th><th>IP</th><th>Session</th><th>Actions</th></tr></thead>
    <tbody id="clientsBody"><tr><td colspan="5" class="dim" style="text-align:center">Loading...</td></tr></tbody></table>
    <div class="btns" style="margin-top:10px"><button class="btn btn-sm btn-blue" onclick="fetchClients()">Refresh</button></div>
  </div>

  <!-- NETWORKS -->
  <div class="panel" id="p-networks">
    <div><span class="prompt">admin@nodex:~$</span> <span class="cmd">show saved-networks</span></div>
    <div class="section-title sky">###### Saved Wi-Fi Networks</div>
    <div class="dim" style="font-size:.82rem;margin-bottom:8px">Up to 5 networks. Click <b style="color:#5b9bd5">Use</b> to set as upstream and reboot NodeX.</div>
    <table><thead><tr><th>#</th><th>SSID</th><th>Actions</th></tr></thead>
    <tbody id="networksBody"><tr><td colspan="3" class="dim" style="text-align:center">Loading...</td></tr></tbody></table>
    <div class="section-title yellow" style="margin-top:18px">###### Add / Update Network</div>
    <div class="form-grid" style="margin-top:10px">
      <div class="card">
        <h3>Save Credentials</h3>
        <div class="form-group"><label>SSID</label><input id="newNetSSID" placeholder="Network name"></div>
        <div class="form-group"><label>Password</label><input id="newNetPass" type="password" placeholder="Leave empty for open networks"></div>
        <div class="btns"><button class="btn btn-green btn-sm" onclick="addNetwork()">Save Network</button></div>
      </div>
    </div>
  </div>

  <!-- CONFIG -->
  <div class="panel" id="p-config">
    <div><span class="prompt">admin@nodex:~$</span> <span class="cmd">edit config</span></div>
    <div class="form-grid" style="margin-top:12px">
      <div class="card">
        <h3>Station (Upstream)</h3>
        <div class="form-group"><label>SSID</label><input id="cfgStaSSID" placeholder="Wi-Fi network name"></div>
        <div class="form-group"><label>Password</label><input id="cfgStaPass" type="password" placeholder="Wi-Fi password"></div>
        <button class="btn btn-sm btn-blue" onclick="scanNetworks()">Scan Networks</button>
        <div id="scanResults" class="hidden" style="margin-top:8px">
          <table><thead><tr><th>SSID</th><th>RSSI</th><th>Ch</th><th></th></tr></thead>
          <tbody id="scanBody"></tbody></table>
        </div>
      </div>
      <div class="card">
        <h3>Access Point</h3>
        <div class="form-group"><label>SSID</label><input id="cfgApSSID" placeholder="NodeX_Repeater"></div>
        <div class="form-group"><label>Channel (1-13)</label><input id="cfgApChan" type="number" min="1" max="13"></div>
        <div class="form-group"><label>Max Clients (1-10)</label><input id="cfgApMax" type="number" min="1" max="10"></div>
      </div>
      <div class="card">
        <h3>Login Codes</h3>
        <div class="form-group"><label>Network Code</label><input id="cfgApPass" placeholder="Current shown below"></div>
        <div class="form-group"><label>Admin Code</label><input id="cfgAdminCode" placeholder="Current shown below"></div>
      </div>
      <div class="card">
        <h3>Dashboard Auth (API)</h3>
        <div class="form-group"><label>Username</label><input id="cfgWebUser"></div>
        <div class="form-group"><label>Password</label><input id="cfgWebPass" type="password"></div>
        <div class="form-group"><label>OTA Password</label><input id="cfgOtaPass" type="password"></div>
      </div>
    </div>
    <div class="btns"><button class="btn btn-green" onclick="saveConfig()">Save Configuration</button></div>
  </div>

  <!-- TOOLS -->
  <div class="panel" id="p-tools">
    <div><span class="prompt">admin@nodex:~$</span> <span class="cmd">show tools</span></div>
    <div class="section-title red">###### Device Actions</div>
    <div class="dim" style="font-size:.85rem;margin-bottom:10px">These actions take effect immediately.</div>
    <div class="btns">
      <button class="btn btn-blue" onclick="rebootDevice()">Reboot</button>
      <button class="btn btn-red" onclick="factoryReset()">Factory Reset</button>
    </div>
    <div class="section-title yellow" style="margin-top:20px">###### OTA Update</div>
    <div style="font-size:.85rem">
      <span class="dim">Use PlatformIO OTA upload.</span><br>
      <span class="dim">Hostname:</span> <span class="white">NodeX</span>
      &nbsp;<span class="dim">Port:</span> <span class="white">3232</span>
    </div>
  </div>
</div>

<div class="status-bar">
  <span><span class="sb-mode">ADMIN</span></span>
  <span class="sb-file" id="sbFile">~/dashboard/status</span>
  <span><span class="sb-info" id="sbHeap"></span> <span class="sb-red">nodex</span></span>
</div>
<div class="toast" id="toast"></div>

<script>
function esc(s){return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');}
function toast(msg,ok){const e=document.getElementById('toast');e.textContent=msg;
  e.className='toast show '+(ok?'ok':'err');setTimeout(()=>e.className='toast',3000);}

// Tabs
document.querySelectorAll('.tab:not(.logout)').forEach(t=>{
  t.addEventListener('click',()=>{
    document.querySelectorAll('.tab').forEach(x=>x.classList.remove('active'));
    t.classList.add('active');
    document.querySelectorAll('.panel').forEach(p=>p.classList.remove('active'));
    document.getElementById('p-'+t.dataset.tab).classList.add('active');
    document.getElementById('sbFile').textContent='~/dashboard/'+t.dataset.tab;
    if(t.dataset.tab==='clients')fetchClients();
    if(t.dataset.tab==='networks')fetchNetworks();
    if(t.dataset.tab==='config')fetchConfig();
  });
});

function fmtUptime(s){const d=Math.floor(s/86400),h=Math.floor(s%86400/3600),m=Math.floor(s%3600/60);
  let str='';if(d)str+=d+'d ';str+=h+'h '+m+'m';return str;}
function rssiPct(r){return Math.min(100,Math.max(0,(r+100)*2));}
function rssiColor(p){return p>60?'#4ec843':p>30?'#f0c040':'#e06060';}
function statLine(lbl,val,cls){return '<div class="stat"><span class="lbl"><span class="bullet">\u25b8</span> '+lbl+'</span><span class="val'+(cls?' '+cls:'')+'">'+val+'</span></div>';}

async function fetchStatus(){
  try{
    const r=await fetch('/api/status');const d=await r.json();
    document.getElementById('sysInfo').innerHTML=
      statLine('Uptime',fmtUptime(d.uptime))+
      statLine('Free Heap',(d.freeHeap/1024).toFixed(1)+' KB / '+(d.heapSize/1024).toFixed(0)+' KB')+
      statLine('Chip',esc(d.chipModel)+' @ '+d.cpuFreqMHz+' MHz')+
      statLine('Flash',d.flashSizeMB+' MB');
    const pct=rssiPct(d.staRSSI);
    document.getElementById('staInfo').innerHTML=
      statLine('Status',d.staConnected?'\u25cf Connected':'\u25cf Disconnected',d.staConnected?'up':'down')+
      statLine('SSID',esc(d.staSSID)||'\u2014')+
      statLine('IP',d.staConnected?esc(d.staIP):'\u2014')+
      statLine('RSSI',d.staRSSI+' dBm <span class="rssi-bar"><span class="fill" style="width:'+pct+'%;background:'+rssiColor(pct)+'"></span></span>')+
      statLine('Gateway',esc(d.staGateway));
    document.getElementById('apInfo').innerHTML=
      statLine('SSID',esc(d.apSSID))+
      statLine('IP',esc(d.apIP))+
      statLine('Channel',d.apChannel)+
      statLine('Clients',d.apClients+' / '+d.apMaxClients);
    document.getElementById('uptimeLabel').textContent='up '+fmtUptime(d.uptime);
    document.getElementById('sbHeap').textContent='heap:'+(d.freeHeap/1024).toFixed(0)+'K';
  }catch(e){console.warn('Status fetch failed',e);}
}

function fmtAge(s){if(s<60)return s+'s';if(s<3600)return Math.floor(s/60)+'m';return Math.floor(s/3600)+'h '+Math.floor((s%3600)/60)+'m';}
async function fetchClients(){
  try{
    const [r1,r2]=await Promise.all([fetch('/api/clients'),fetch('/api/sessions')]);
    const clients=await r1.json();const sessions=await r2.json();
    const sessMap={};sessions.forEach(s=>{sessMap[s.ip]={age:s.age};});
    const tb=document.getElementById('clientsBody');
    const rows=[];const seen=new Set();
    clients.forEach((c,i)=>{
      const sess=sessMap[c.ip];const hasAuth=!!sess;
      rows.push('<tr>'+
        '<td>'+(i+1)+'</td>'+
        '<td>'+esc(c.mac)+'</td>'+
        '<td>'+esc(c.ip)+'</td>'+
        '<td>'+(hasAuth?'<span class="ok">&#9679; Auth</span> <span class="dim">('+fmtAge(sess.age)+')</span>':'<span class="dim">&#9675; Not Auth</span>')+'</td>'+
        '<td style="white-space:nowrap">'+(hasAuth?'<button class="btn btn-sm btn-blue" style="margin-right:4px" onclick="deauthSession(\''+c.ip+'\')" >Deauth</button>':'')+
          '<button class="btn btn-sm btn-red" onclick="kickClient(\''+c.mac+'\')" >Kick</button>'+
        '</td></tr>');
      seen.add(c.ip);
    });
    sessions.filter(s=>!seen.has(s.ip)).forEach(s=>{
      rows.push('<tr><td>&#8212;</td><td><span class="dim">?</span></td><td>'+esc(s.ip)+'</td>'+
        '<td><span class="ok">&#9679; Auth</span> <span class="dim">('+fmtAge(s.age)+')</span></td>'+
        '<td><button class="btn btn-sm btn-blue" onclick="deauthSession(\''+s.ip+'\')" >Deauth</button></td></tr>');
    });
    if(!rows.length){tb.innerHTML='<tr><td colspan="5" class="dim" style="text-align:center">No devices</td></tr>';return;}
    tb.innerHTML=rows.join('');
  }catch(e){console.warn('Clients fetch failed',e);}
}
async function deauthSession(ip){
  if(!confirm('Revoke session for '+ip+'?\nThey will see the captive portal again.'))return;
  try{const r=await fetch('/api/sessions/deauth',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'ip='+encodeURIComponent(ip)});
    const d=await r.json();if(r.ok){toast('Session revoked \u2014 client will re-authenticate',true);fetchClients();}else toast(d.error||'Failed',false);
  }catch(e){toast('Request failed',false);}
}
async function kickClient(mac){
  if(!confirm('Kick '+mac+' from Wi-Fi?\nThey will be disconnected.'))return;
  try{const r=await fetch('/api/clients/kick',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'mac='+encodeURIComponent(mac)});
    const d=await r.json();if(r.ok){toast('Client kicked',true);setTimeout(fetchClients,600);}else toast(d.error||'Not found',false);
  }catch(e){toast('Request failed',false);}
}
async function fetchNetworks(){
  try{const r=await fetch('/api/networks');const arr=await r.json();
    const tb=document.getElementById('networksBody');
    if(!arr.length){tb.innerHTML='<tr><td colspan="3" class="dim" style="text-align:center">No saved networks</td></tr>';return;}
    tb.innerHTML=arr.map((n,i)=>'<tr><td>'+(i+1)+'</td><td>'+esc(n.ssid)+'</td>'+
      '<td style="white-space:nowrap"><button class="btn btn-sm btn-blue" style="margin-right:4px" onclick="connectSaved('+i+')">Use</button>'+
      '<button class="btn btn-sm btn-red" onclick="removeNetwork('+i+')">Remove</button></td></tr>').join('');
  }catch(e){console.warn('Networks fetch failed',e);}
}
async function addNetwork(){
  const ssid=document.getElementById('newNetSSID').value.trim();
  const pass=document.getElementById('newNetPass').value;
  if(!ssid){toast('SSID is required',false);return;}
  try{const r=await fetch('/api/networks/add',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid,pass})});
    const d=await r.json();if(r.ok){toast('Network saved',true);document.getElementById('newNetSSID').value='';document.getElementById('newNetPass').value='';fetchNetworks();}else toast(d.error||'Failed',false);
  }catch(e){toast('Request failed',false);}
}
async function removeNetwork(idx){
  if(!confirm('Remove this saved network?'))return;
  try{const r=await fetch('/api/networks/remove',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'index='+idx});
    const d=await r.json();if(r.ok){toast('Removed',true);fetchNetworks();}else toast(d.error||'Failed',false);
  }catch(e){toast('Request failed',false);}
}
async function connectSaved(idx){
  if(!confirm('Set this as the upstream network and reboot NodeX?'))return;
  try{const r=await fetch('/api/networks/connect',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'index='+idx});
    const d=await r.json();if(r.ok)toast('Switching network \u2014 rebooting\u2026',true);else toast(d.error||'Failed',false);
  }catch(e){toast('Request failed',false);}
}

async function fetchConfig(){
  try{
    const r=await fetch('/api/config');const d=await r.json();
    document.getElementById('cfgStaSSID').value=d.staSSID||'';
    document.getElementById('cfgApSSID').value=d.apSSID||'';
    document.getElementById('cfgApChan').value=d.apChannel||1;
    document.getElementById('cfgApMax').value=d.apMaxClients||8;
    document.getElementById('cfgApPass').placeholder=d.apPass||'';
    document.getElementById('cfgAdminCode').placeholder=d.adminCode||'';
  }catch(e){}
}

async function saveConfig(){
  const v=id=>document.getElementById(id).value;
  const body={};
  if(v('cfgStaSSID'))body.staSSID=v('cfgStaSSID');
  if(v('cfgStaPass'))body.staPass=v('cfgStaPass');
  if(v('cfgApSSID'))body.apSSID=v('cfgApSSID');
  if(v('cfgApPass'))body.apPass=v('cfgApPass');
  if(v('cfgAdminCode'))body.adminCode=v('cfgAdminCode');
  if(v('cfgApChan'))body.apChannel=parseInt(v('cfgApChan'));
  if(v('cfgApMax'))body.apMaxClients=parseInt(v('cfgApMax'));
  if(v('cfgWebUser'))body.webUser=v('cfgWebUser');
  if(v('cfgWebPass'))body.webPass=v('cfgWebPass');
  if(v('cfgOtaPass'))body.otaPass=v('cfgOtaPass');
  try{
    const r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
    const d=await r.json();
    if(r.ok)toast('Configuration saved \u2014 reboot to apply',true);
    else toast(d.error||'Save failed',false);
  }catch(e){toast('Save failed',false);}
}

async function scanNetworks(){
  document.getElementById('scanResults').classList.remove('hidden');
  document.getElementById('scanBody').innerHTML='<tr><td colspan="4" class="dim">Scanning\u2026</td></tr>';
  try{
    await fetch('/api/scan');
    setTimeout(async()=>{
      const r=await fetch('/api/scan');const arr=await r.json();
      if(arr.status==='scanning'){document.getElementById('scanBody').innerHTML='<tr><td colspan="4" class="dim">Still scanning\u2026 try again</td></tr>';return;}
      if(!arr.length){document.getElementById('scanBody').innerHTML='<tr><td colspan="4" class="dim">No networks found</td></tr>';return;}
      document.getElementById('scanBody').innerHTML=arr.map(n=>
        '<tr><td>'+esc(n.ssid)+'</td><td>'+n.rssi+'</td><td>'+n.channel+'</td><td><button class="btn btn-sm btn-blue" data-ssid="'+esc(n.ssid)+'" onclick="document.getElementById(\'cfgStaSSID\').value=this.dataset.ssid">Select</button></td></tr>'
      ).join('');
    },4000);
  }catch(e){toast('Scan failed',false);}
}

async function rebootDevice(){
  if(!confirm('Reboot NodeX?'))return;
  try{await fetch('/api/reboot',{method:'POST'});toast('Rebooting\u2026',true);}catch(e){toast('Reboot failed',false);}
}
async function factoryReset(){
  if(!confirm('FACTORY RESET \u2014 all settings will be erased. Continue?'))return;
  try{await fetch('/api/factory-reset',{method:'POST'});toast('Factory reset \u2014 rebooting\u2026',true);}catch(e){toast('Reset failed',false);}
}
function logout(){
  fetch('/portal/logout',{method:'POST'}).then(()=>{window.location.href='/';}).catch(()=>{window.location.href='/';});
}

fetchStatus();
setInterval(fetchStatus,5000);
</script>
</body>
</html>
)rawliteral";
