function change_day(day) {
    let week_day;

    switch (day) {
        case 0:
            week_day = '星期日';
            break;
        case 1:
            week_day = '星期一';
            break;
        case 2:
            week_day = '星期二';
            break;
        case 3:
            week_day = '星期三';
            break;
        case 4:
            week_day = '星期四';
            break;
        case 5:
            week_day = '星期五';
            break;
        case 6:
            week_day = '星期六';
            break;
        default:
            break;
    }
    return week_day;
}

function fresh() {


    let now = new Date();

    let time_zone = Number(document.getElementById('timezone').value) - 8;

    now.setUTCHours(now.getHours() + time_zone);

    let temp;

    if (change12_24 === true) {
        temp = now.getUTCHours() % 24;
    } else if (change12_24 === false) {
        temp = (now.getUTCHours() === 0||now.getUTCHours() === 12 )? 12 : now.getUTCHours() % 12;
    }

    let hour = temp < 10 ? '0' + temp : temp;

    let minute = now.getMinutes() < 10 ? '0' + now.getMinutes() : now.getMinutes();
    let second = now.getSeconds() < 10 ? '0' + now.getSeconds() : now.getSeconds();

    let year = now.getUTCFullYear();
    let month = now.getUTCMonth() + 1;
    let date = now.getUTCDate();

    let day = now.getUTCDay();

    time_clock.innerText = hour + ':' + minute + ':' + second;
    time_date.innerText = year + '年' + month + '月' + date + '日';
    time_period.innerText = hour < 12 ? '上午' : '下午';
    time_day.innerText = change_day(day);
}

let change12_24 = true;

let time_clock = document.getElementById('time');
let time_date = document.getElementById('date');
let time_period = document.getElementById('period');
let time_day = document.getElementById('day');
time_period.style.visibility = 'hidden';

let submit_button = document.getElementById('switch');

const handler = () => {
    if (submit_button.innerText === '切换为24小时制') {
        submit_button.innerText = '切换为12小时制';
        change12_24 = true;
        time_period.style.visibility = 'hidden';

    } else if (submit_button.innerText === '切换为12小时制') {
        submit_button.innerText = '切换为24小时制';
        time_period.style.visibility = 'visible';
        change12_24 = false;        
    }
}

window.addEventListener('keydown', function (event) {
    if (event.code === 'Space') {
        handler();
    }
}, true)

submit_button.addEventListener('click', handler);

setInterval('fresh()', 100);