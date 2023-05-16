import chartsConfig from "./chart-config";

const dailyTemperatureChart = {
  type: "line",
  height: 220,
  series: [
    {
      name: "Temperature",
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

export default dailyTemperatureChart;
