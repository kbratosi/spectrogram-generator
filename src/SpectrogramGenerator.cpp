#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const int sample_rate) {
  data_ = nullptr;
  data_size_ = 0;
  sample_rate_ = sample_rate;
}

int SpecGen::decodeAudioFile(const char *file_name) {
  // initialize muxers, demuxers, protocols for libavformat
  av_register_all();

  // get file format
  AVFormatContext *format = avformat_alloc_context();
  if(avformat_open_input(&format, file_name, nullptr, nullptr) != 0) {
    fprintf(stderr, "Could not open file '%s'\n", file_name);
    return -1;
  }
  if (avformat_find_stream_info(format, NULL) < 0) {
    fprintf(stderr, "Could not retrieve stream info from file '%s'\n", file_name);
    return -1;
  }

  // Find the index of the first audio stream
  int stream_index = -1;
  for(int i = 0; i < format->nb_streams; i++) {
    if(format->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
      stream_index = i;
      break;
    }
  }
  if (stream_index == -1) {
    fprintf(stderr, "Could not retrieve audio stream from file '%s'\n", file_name);
    return -1;
  }
  AVStream* stream = format->streams[stream_index];

  // find & open codec
  AVCodecContext* codec = stream->codec;
  if (avcodec_open2(codec, avcodec_find_decoder(codec->codec_id), NULL) < 0) {
    fprintf(stderr, "Failed to open decoder for stream #%u in file '%s'\n", stream_index, file_name);
    return -1;
  }

  // prepare resampler
  struct SwrContext* swr = swr_alloc();
  av_opt_set_int(swr, "in_channel_count",  codec->channels, 0);
  av_opt_set_int(swr, "out_channel_count", 1, 0);
  av_opt_set_int(swr, "in_channel_layout",  codec->channel_layout, 0);
  av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
  av_opt_set_int(swr, "in_sample_rate", codec->sample_rate, 0);
  av_opt_set_int(swr, "out_sample_rate", sample_rate_, 0);
  av_opt_set_sample_fmt(swr, "in_sample_fmt",  codec->sample_fmt, 0);
  av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_DBL,  0);
  swr_init(swr);
  if (!swr_is_initialized(swr)) {
    fprintf(stderr, "Resampler has not been properly initialized\n");
    return -1;
  }

  // prepare to read data
  AVPacket packet;
  av_init_packet(&packet);

  AVFrame* frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Error allocating the frame\n");
		return -1;
	}

  // clean up
  av_frame_free(&frame);
  swr_free(&swr);
  avcodec_close(codec);
  avformat_free_context(format);

  // success
  return 0;
}