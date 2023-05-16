import chartsConfig from "./chart-config";

const dailyHumidityChart = {
  type: "line",
  height: 220,
  series: [
    {
      name: "Humidity",
      data: [50, 40, 300, 320, 500, 350, 200, 230, 500],
    },
  ],
  options: {
    ...chartsConfig,
    colors: ["#fff"],
    stroke: {
      lineCap: "round",
    },
    markers: {
      size: 5,
    },
    xaxis: {
      ...chartsConfig.xaxis,
    },
  },
};

export default dailyHumidityChart;
