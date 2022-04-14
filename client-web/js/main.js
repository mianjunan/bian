const ws=new WebSocket('ws://' + location.host + '/message');

let button;

window.onload = function() {
    if(getPar('type')==='ADMIN'){
        document.getElementById('normal_button').style.display='none';
        document.getElementById('admin_button').style.display='block';
    }
}



function join(){
    switch (button) {
        case 'login':


    }
}



function getPar(par){
    const local_url = document.location.href;
    const get = local_url.indexOf(par + "=");
    if(get === -1){
        return false;
    }
    let get_par = local_url.slice(par.length + get + 1);
    const nextPar = get_par.indexOf("&");
    if(nextPar !== -1){
        get_par = get_par.slice(0, nextPar);
    }
    return get_par;
}